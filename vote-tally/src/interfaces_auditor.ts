import { TallyStats } from "./interfaces";

export interface AuditorCandidate {
    candidate_name: string;
    locked_tokens: string;
    total_votes: number;
    is_active: boolean;
    unstaking_end_time_stamp: string;
}

export interface AuditorVote {
    voter: string;
    proxy: string;
    staked: number;
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
    bio: AuditorBio;
    is_auditor: boolean;
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

export interface AuditorAuditors {
    auditor_name: string;
}

export interface AuditorVoteJSON {
    voter: string;
    vote_json: string;
    updated_at: string;
}

export interface AuditorVotesCombined extends AuditorVote {
    vote_json?: {
        comment?: string;
        [key: string]: any;
    };
    updated_at?: string;
}

export interface AuditorBios {
    candidate_name: string;
    bio: string;
}

export interface AuditorConfig {
    lockupasset: string;
    maxvotes: number;
    numelected: number;
    authaccount: number;
    auth_threshold_auditors: number;
    lockup_release_time_delay: number;
}

export interface AuditorBio {
    avatar: string;
    bio: string;
    contact: string;
    [key: string]: string;
}
