export interface Voters {
    owner: string;
    proxy: string;
    producers: any[];
    staked: number;
    last_vote_weight: string;
    proxied_vote_weight: string;
    is_proxy: number;
    flags1: number;
    reserved2: number;
    reserved3: string;
}

export interface Vote {
    id: number;
    proposal_name: string;
    voter: string;
    vote: number;
    vote_json: string;
    updated_at: string;
}

export interface Proposal {
    proposal_name: string;
    proposer: string;
    title: string;
    proposal_json: string;
    created_at: string;
    expires_at: string;
}

export interface Delband {
    from: string;
    to: string;
    net_weight: string;
    cpu_weight: string;
}

export interface Userres {
    ram_bytes: number;
    cpu_weight: string;
    net_weight: string;
    owner: string;
}

export interface Tallies {
    /**
     * proposal_name
     */
    [proposal_name: string]: Tally,
}

export interface Tally {
    id: string;
    stats: Stats;
    proposal: Proposal;
}

export interface Stats {
    /**
     * Block Number used for Tally calculations
     */
    block_num: number;
    /**
     * Currency Supply used for Tally calculations
     */
    currency_supply: number;
    /**
     * Total number of votes per account & proxies
     */
    votes: {
        [vote: number]: number
        proxies: number,
        accounts: number,
        total: number,
    };
    /**
     * Accounts Staked
     *
     * Staked weight is calculated using `voter_info.staked` or `self_delegated_bandwidth`
     */
    accounts: {
        [vote: number]: number
        total: number,
    };
    /**
     * Proxies Staked
     *
     * Whenever a proxy votes on a proposal, a sum of each account's staked which have NOT voted for a proposal will counted.
     */
    proxies: {
        [vote: number]: number
        total: number,
    };
    /**
     * Total Staked between both accounts & proxies
     */
    staked: {
        [vote: number]: number
        total: number,
    };
}

export interface ProxiesVote extends Vote {
    staked_proxy: number;
}

export interface Proxies {
    /**
     * Account Information
     */
    [account_name: string]: {
        votes: {
            [proposal_name: string]: ProxiesVote;
        }
        staked: number;
        is_proxy: boolean;
        proxy: string;
    }
}

export interface Accounts {
    /**
     * Account Information
     */
    [account_name: string]: {
        votes: {
            [proposal_name: string]: Vote;
        }
        staked: number;
        is_proxy: boolean;
        proxy: string;
    }
}

export interface CurrencyStats {
    [symbol: string]: {
        supply:     string;
        max_supply: string;
        issuer:     string;
    }
}