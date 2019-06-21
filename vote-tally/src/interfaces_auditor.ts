import { TallyStats } from "./interfaces";

export interface AuditorCandidate {
    candidate_name: string;
    locked_tokens: string;
    // total_votes: number;
    is_active: boolean;
    auditor_end_time_stamp: string;
}

export interface AuditorVote {
    voter: string;
    candidates: string[];
}

export interface AuditorProxiesVote extends AuditorVote {
    staked_proxy: number;
}


export interface AuditorTallies {
    /**
     * candidate_name
     */
    [candidate_name: string]: AuditorTally,
}

export interface AuditorTally {
    id: string;
    stats: TallyStats;
    candidate: AuditorCandidate;
}

export interface AuditorProxiesVote extends AuditorVote {
    staked_proxy: number;
}

export interface AuditorProxies {
    /**
     * Account Information
     */
    [account_name: string]: {
        votes: {
            [candidate_name: string]: AuditorProxiesVote
        }
        staked: number;
        is_proxy: boolean;
        proxy: string;
    }
}

export interface AuditorAccounts {
    /**
     * Account Information
     */
    [account_name: string]: {
        votes: {
            [candidate_name: string]: AuditorVote
        }
        staked: number;
        is_proxy: boolean;
        proxy: string;
    }
}
