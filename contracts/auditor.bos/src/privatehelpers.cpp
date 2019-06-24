
contr_config auditorbos::configs() {
    contr_config conf = config_singleton.get_or_default(contr_config());
    config_singleton.set(conf, _self);
    return conf;
}
