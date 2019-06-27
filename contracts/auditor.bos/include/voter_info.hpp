struct [[eosio::table, eosio::contract("eosio.system")]] voter_info {
    name                owner;     /// the voter
    name                proxy;     /// the proxy set by the voter, if any
    std::vector<name>   producers; /// the producers approved by this voter if no proxy set
    int64_t             staked = 0;

    /**
     *  Every time a vote is cast we must first "undo" the last vote weight, before casting the
     *  new vote weight.  Vote weight is calculated as:
     *
     *  stated.amount * 2 ^ ( weeks_since_launch/weeks_per_year)
     */
    double              last_vote_weight = 0; /// the vote weight cast the last time the vote was updated

    /**
     * Total vote weight delegated to this voter.
     */
    double              proxied_vote_weight = 0; /// the total vote weight delegated to this voter as a proxy
    bool                is_proxy = 0; /// whether the voter is a proxy for others


    uint32_t            flags1 = 0;
    uint32_t            reserved2 = 0;
    eosio::asset        reserved3;

    uint64_t primary_key() const { return owner.value; }

    enum class flags1_fields : uint32_t {
        ram_managed = 1,
        net_managed = 2,
        cpu_managed = 4
    };
};

typedef eosio::multi_index< "voters"_n, voter_info > voter_table;