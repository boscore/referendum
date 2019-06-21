import { EosioVoter, EosioDelband, ForumAccounts, ForumVote, ForumProxies, ForumProposal, ForumTallies, ForumTally } from "./interfaces";
import { defaultAccount, countStaked, defaultStats } from "./tallies";

export function generateForumProxies(votes: ForumVote[], delband: EosioDelband[], voters: EosioVoter[]): ForumProxies {
    const accounts = generateForumAccounts(votes, delband, voters, false);
    const accountsProxies: any = generateForumAccounts(votes, delband, voters, true);

    for (const proxyName of Object.keys(accountsProxies)) {
        const proxy = accountsProxies[proxyName];

        for (const proposalName of Object.keys(proxy.votes)) {
            // Initialize `proxy_staked` for each proposal using self delegated EOS from proxy
            proxy.votes[proposalName].staked_proxy = Number(proxy.staked);

            for (const accountName of Object.keys(accounts)) {
                const account = accounts[accountName];

                // Check if account belongs to proxy
                if (account.proxy !== proxyName) continue;

                // Check if user has already voted for proposal
                // Do not add user `stake` if already voted for same proposal
                if (account.votes[proposalName]) continue;

                // Add user `staked` to `staked_proxy`
                accountsProxies[proxyName].votes[proposalName].staked_proxy += Number(account.staked);
            }
        }
    }

    return accountsProxies;
}

export function generateForumAccounts(votes: ForumVote[], delband: EosioDelband[], voters: EosioVoter[], proxies = false): ForumAccounts {
    const accounts: ForumAccounts = {};
    const voted = new Set(); // track who has voted

    // Only track accounts who has casted votes
    for (const row of votes) {
        if (!accounts[row.voter]) accounts[row.voter] = defaultAccount();

        const account = accounts[row.voter];
        if (account.votes) account.votes[row.proposal_name] = row;
        voted.add(row.voter);
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

export function generateForumTallies(block_num: number, proposals: ForumProposal[], accounts: ForumAccounts, proxies: ForumAccounts, currency_supply: number): ForumTallies {
    const tallies: ForumTallies = {};

    for (const proposal of proposals) {
        tallies[proposal.proposal_name] = generateForumTally(block_num, proposal, accounts, proxies, currency_supply);
    }
    return tallies;
}

export function generateForumTally(block_num: number, proposal: ForumProposal, accounts: ForumAccounts, proxies: ForumAccounts, currency_supply: number): ForumTally {
    const { proposal_name } = proposal;
    const stats = defaultStats(block_num);

    // Calculate account's staked
    for (const owner of Object.keys(accounts)) {
        const account = accounts[owner];
        const { votes } = account;
        const staked = Number(account.staked);

        if (votes[proposal_name]) {
            const { vote } = votes[proposal_name];
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

        if (votes[proposal_name]) {
            const { vote } = votes[proposal_name];
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
        if (!proxyAccount.votes[proposal_name]) continue;

        const { vote } = proxyAccount.votes[proposal_name];
        let staked = 0;

        for (const owner of Object.keys(accounts)) {
            const account = accounts[owner];

            // Skip user isn't using this proxy
            if (account.proxy !== proxy) continue;

            // Skip user has already voted
            if (account.votes[proposal_name]) continue;

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

    // Proposal unique ID
    // ProposalName_YYYYMMDD // awesomeprop_20181206
    const date = proposal.created_at.split("T")[0].replace(/-/g, "");
    const id = `${proposal_name}_${date}`;

    return {
        id,
        proposal,
        stats,
    };
}