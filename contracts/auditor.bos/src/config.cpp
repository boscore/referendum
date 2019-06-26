void auditorbos::updateconfig(contr_config new_config) {
    require_auth(_self);

    check(new_config.auth_threshold_auditors < new_config.numelected,
                 "ERR::UPDATECONFIG_INVALID_AUTH_AUDITORS_TO_NUM_ELECTED::The auth threshold can never be satisfied with a value greater than the number of elected auditors");

    _config.set(new_config, _self);
}

contr_config auditorbos::configs() {
    contr_config conf = _config.get_or_default(contr_config());
    _config.set(conf, _self);
    return conf;
}
