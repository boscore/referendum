void auditorbos::transfer( name from,
                           name to,
                           asset quantity,
                           const std::string& memo ) {

    if (to == _self) {
        const auto & reg_candidate = _candidates.find(from.value);
        const time_point_sec unstaking_period = current_time_point() + time_point_sec(configs().lockup_release_time_delay);

        // Modify existing registered candidate with additional locked_tokens
        if (reg_candidate != _candidates.end()) {
            _candidates.modify(reg_candidate, eosio::same_payer, [&](auto & row) {
                row.locked_tokens += quantity;
                row.unstaking_end_time_stamp = unstaking_period;
            });
        // New candidate, register them and add locked_tokens
        } else {
            _candidates.emplace(_self, [&](auto & row) {
                row.candidate_name = from;
                row.locked_tokens = quantity;
                row.total_votes = 0;
                row.is_active = 0;
                row.unstaking_end_time_stamp = unstaking_period;
            });
        }
    }
}