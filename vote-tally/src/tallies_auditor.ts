import { EosioVoter, EosioDelband, AuditorAccounts, AuditorVote, AuditorProxies, AuditorTallies, AuditorTally, TallyStats } from "./interfaces";
import { AuditorCandidate, AuditorBios, AuditorVotesCombined, AuditorVoteJSON, AuditorAuditors } from "./interfaces_auditor";
import { countStaked, defaultStats, defaultAccount } from "./tallies";

export function generateAuditorProxies(votes: AuditorVote[], delband: EosioDelband[], voters: EosioVoter[]): AuditorProxies {
    const accounts = generateAuditorAccounts(votes, delband, voters, false);
    const accountsProxies: any = generateAuditorAccounts(votes, delband, voters, true);

    for (const proxyName of Object.keys(accountsProxies)) {
        const proxy = accountsProxies[proxyName];

        for (const candidate_name of Object.keys(proxy.votes)) {
            // Initialize `proxy_staked` for each proposal using self delegated EOS from proxy
            proxy.votes[candidate_name].staked_proxy = Number(proxy.staked);

            for (const accountName of Object.keys(accounts)) {
                const account = accounts[accountName];

                // Check if account belongs to proxy
                if (account.proxy !== proxyName) continue;

                // Check if user has already voted for proposal
                // Do not add user `stake` if already voted for same proposal
                if (account.votes[candidate_name]) continue;

                // Add user `staked` to `staked_proxy`
                accountsProxies[proxyName].votes[candidate_name].staked_proxy += Number(account.staked);
            }
        }
    }

    return accountsProxies;
}

export function generateAuditorAccounts(votes: AuditorVote[], delband: EosioDelband[], voters: EosioVoter[], proxies = false): AuditorAccounts {
    const accounts: AuditorAccounts = {};
    const voted = new Set(); // track who has voted

    // Only track accounts who has casted votes
    for (const row of votes) {
        if (!accounts[row.voter]) accounts[row.voter] = defaultAccount();

        const account = accounts[row.voter];

        for (const candidate_name of row.candidates) {
            account.votes[candidate_name] = row;
            voted.add(row.voter);
        }
    }

    // Load Voter Information
    for (const row of voters) {
        const owner = row.owner;

        // Voter is only included if voted or proxied to a proxy who has voted
        if (voted.has(owner) || voted.has(row.proxy)) {
            if (!accounts[owner]) accounts[owner] = defaultAccount();
            accounts[owner].staked = row.staked;
            accounts[owner].is_proxy = Boolean(row.is_proxy);
            accounts[owner].proxy = row.proxy;
        }
    }

    // Load Self Delegated Bandwidth
    for (const row of delband) {
        const owner = row.from;
        if (!accounts[owner]) accounts[owner] = defaultAccount();
        if (!accounts[owner].staked) accounts[owner].staked = countStaked(row);
    }

    // Remove/Include proxies
    for (const owner of Object.keys(accounts)) {
        const account = accounts[owner];

        // Proxies
        if (proxies !== account.is_proxy) delete accounts[owner];
    }

    return accounts;
}

export function generateAuditorTallies(block_num: number, candidates: AuditorCandidate[], accounts: AuditorAccounts, proxies: AuditorAccounts, auditor_bios: AuditorBios[], auditor_auditors: AuditorAuditors[]): AuditorTallies {
    const tallies: AuditorTallies = {};

    for (const candidate of candidates) {
        tallies[candidate.candidate_name] = generateAuditorTally(block_num, candidate, accounts, proxies, auditor_bios, auditor_auditors);
    }
    return tallies;
}

export function combineAuditorVotes(auditor_votes: AuditorVote[], auditor_votejson: AuditorVoteJSON[]): AuditorVotesCombined[] {
    const votes: {[voter: string]: AuditorVotesCombined} = {};

    for (const vote of auditor_votes) {
        votes[vote.voter] = vote;
        votes[vote.voter].updated_at = "";
        votes[vote.voter].vote_json = {};
    }

    for (const votejson of auditor_votejson) {
        votes[votejson.voter].updated_at = votejson.updated_at;
    
        // Handle JSON.parse
        try {
            votes[votejson.voter].vote_json = JSON.parse(votejson.vote_json);
        } catch {
            // JSON.parse error
        }
    }

    return Object.values(votes);
}

export function generateAuditorTally(block_num: number, candidate: AuditorCandidate, accounts: AuditorAccounts, proxies: AuditorAccounts, auditor_bios: AuditorBios[], auditor_auditors: AuditorAuditors[]): AuditorTally {
    const { candidate_name } = candidate;
    const stats = defaultStats(block_num);

    // Calculate account's staked
    for (const owner of Object.keys(accounts)) {
        const account = accounts[owner];
        const { votes } = account;
        const staked = Number(account.staked);

        if (votes[candidate_name]) {
            // Default vote to 1 = yes
            const vote = 1;

            // Set to 0 if undefined
            if (!stats.accounts[vote]) stats.accounts[vote] = 0;
            if (!stats.staked[vote]) stats.staked[vote] = 0;
            if (!stats.votes[vote]) stats.votes[vote] = 0;

            // Add voting weights
            stats.accounts[vote] += staked;
            stats.accounts.total += staked;
            stats.staked[vote] += staked;
            stats.staked.total += staked;

            // Voting Count
            stats.votes[vote] += 1;
            stats.votes.total += 1;
            stats.votes.accounts += 1;
        }
    }
    // Calculate proxies's staked
    // TO-DO: Create a method to support both accounts & proxies staked portion (removes 15 lines of code)
    for (const owner of Object.keys(proxies)) {
        const account = proxies[owner];
        const { votes } = account;
        const staked = Number(account.staked);

        if (votes[candidate_name]) {
            // Default vote to 1 = yes
            const vote = 1;

            // Set to 0 if undefined
            if (!stats.proxies[vote]) stats.proxies[vote] = 0;
            if (!stats.staked[vote]) stats.staked[vote] = 0;
            if (!stats.votes[vote]) stats.votes[vote] = 0;

            // Add voting weights
            stats.proxies[vote] += staked;
            stats.proxies.total += staked;
            stats.staked[vote] += staked;
            stats.staked.total += staked;

            // Voting Count
            stats.votes[vote] += 1;
            stats.votes.total += 1;
            stats.votes.proxies += 1;
        }
    }

    // Additional proxied staked weights via account's staked who have no voted
    for (const proxy of Object.keys(proxies)) {
        const proxyAccount = proxies[proxy];
        // Skip proxy, did not vote for proposal
        if (!proxyAccount.votes[candidate_name]) continue;

        // Default vote to 1 = yes
        const vote = 1;
        let staked = 0;

        for (const owner of Object.keys(accounts)) {
            const account = accounts[owner];

            // Skip user isn't using this proxy
            if (account.proxy !== proxy) continue;

            // Skip user has already voted
            if (account.votes[candidate_name]) continue;

            // Add user's stake to proxies
            staked += Number(account.staked);
        }

        // Set to 0 if undefined
        if (!stats.proxies[vote]) stats.proxies[vote] = 0;
        if (!stats.staked[vote]) stats.staked[vote] = 0;

        stats.proxies[vote] += staked;
        stats.proxies.total += staked;
        stats.staked[vote] += staked;
        stats.staked.total += staked;
    }

    //  Add Candidate Bio string to Tally
    let bio: any = {
        avatar: "",
        bio: "",
        contact: "",
    };
    for (const auditor_bio of auditor_bios) {
        if (candidate_name === auditor_bio.candidate_name) {
            try {
                bio = Object.assign(bio, JSON.parse(auditor_bio.bio));
            } catch {
                // JSON.parse caused an error
            }
        }
    }

    // Include total votes from stats
    candidate.total_votes = stats.staked.total;

    // Check if candidate is an active auditor
    let is_auditor = false;
    for (const {auditor_name} of auditor_auditors) {
        if (auditor_name === candidate_name) is_auditor = true;
    }

    return {
        id: candidate_name,
        candidate,
        stats,
        bio,
        is_auditor,
    };
}