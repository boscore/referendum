export * from "./interfaces_forum";
export * from "./interfaces_auditor";

export interface EosioVoter {
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

export interface EosioDelband {
    from: string;
    to: string;
    net_weight: string;
    cpu_weight: string;
}

export interface EosioUserres {
    ram_bytes: number;
    cpu_weight: string;
    net_weight: string;
    owner: string;
}

export interface EosioTokenCurrencyStats {
    [symbol: string]: {
        supply:     string;
        max_supply: string;
        issuer:     string;
    }
}

export interface EosioStats {
    /**
     * Block Number used for Summaries calculations
     */
    block_num: number;
    /**
     * Total amount of staked BOS used to vote for Block Producers
     */
    bp_votes: number;
    /**
     * Total amount of staked BOS used to vote for Block Producers by voters (vote weights by individual voters)
     * > If Proxy has staked BOS, that staked amount will be counted towards `bp_producers_votes `
     */
    bp_producers_votes: number;
    /**
     * Total amount of proxied staked BOS used to vote for Block Producers (vote weight to proxies)
     */
    bp_proxy_votes: number;
    /**
     * Currency Supply of `eosio.token`
     */
    currency_supply: number;
}

export interface TallyStats {
    /**
     * Block Number used for Tally calculations
     */
    block_num: number;
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
