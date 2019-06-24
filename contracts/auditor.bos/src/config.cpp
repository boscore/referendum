void auditorbos::updateconfig(contr_config new_config) {
    require_auth(_self);

    check(new_config.auth_threshold_auditors < new_config.numelected,
                 "ERR::UPDATECONFIG_INVALID_AUTH_AUDITORS_TO_NUM_ELECTED::The auth threshold can never be satisfied with a value greater than the number of elected auditors");

    config_singleton.set(new_config, _self);
}
