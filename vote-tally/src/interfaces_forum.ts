import { TallyStats } from "./interfaces";

export interface ForumVote {
    id: number;
    proposal_name: string;
    voter: string;
    vote: number;
    vote_json: string;
    updated_at: string;
}

export interface ForumProxiesVote extends ForumVote {
    staked_proxy: number;
}

export interface ForumProposal {
    proposal_name: string;
    proposer: string;
    title: string;
    proposal_json: string;
    created_at: string;
    expires_at: string;
}

export interface ForumTallies {
    /**
     * proposal_name
     */
    [proposal_name: string]: ForumTally,
}

export interface ForumTally {
    id: string;
    stats: TallyStats;
    proposal: ForumProposal;
}

export interface ForumProxies {
    /**
     * Account Information
     */
    [account_name: string]: {
        votes: {
            [proposal_name: string]: ForumProxiesVote;
        }
        staked: number;
        is_proxy: boolean;
        proxy: string;
    }
}

export interface ForumAccounts {
    /**
     * Account Information
     */
    [account_name: string]: {
        votes: {
            [proposal_name: string]: ForumVote;
        }
        staked: number;
        is_proxy: boolean;
        proxy: string;
    }
}
