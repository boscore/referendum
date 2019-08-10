void auditorbos::transfer( name from,
                           name to,
                           asset quantity,
                           const std::string& memo ) {

    if (to == get_self()) {
        const auto & candidate_itr = _candidates.find(from.value);
        const time_point_sec unstaking_period = current_time_point() + time_point_sec(configs().lockup_release_time_delay);

        bool is_active = false;

        // Modify existing registered candidate with additional locked_tokens
        if (candidate_itr != _candidates.end()) {

            // Auto nominate candidate if locked_tockens is above minimum threshold
            if (candidate_itr->locked_tokens + quantity >= configs().lockupasset) is_active = true;

            // Modify locked_tokens amount & set new unstaking period
            _candidates.modify(candidate_itr, eosio::same_payer, [&](auto & row) {
                row.locked_tokens += quantity;
                row.unstaking_end_time_stamp = unstaking_period;
                row.is_active = is_active;
                row.total_votes = 0;
            });
        // New candidate, register them and add locked_tokens
        } else {
            // Auto nominate candidate if locked_tockens is above minimum threshold
            if (quantity >= configs().lockupasset) is_active = true;

            // Add candidate to table
            _candidates.emplace( get_self(), [&](auto & row) {
                row.candidate_name = from;
                row.locked_tokens = quantity;
                row.total_votes = 0;
                row.is_active = is_active;
                row.unstaking_end_time_stamp = unstaking_period;
            });
        }
    }
}