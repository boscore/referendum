
void auditorbos::transfer(name from,
                          name to,
                          asset quantity,
                          string memo) {
    if (to == _self) {
        pendingstake_table_t pendingstake(_self, _self.value);
        auto source = pendingstake.find(from.value);
        if (source != pendingstake.end()) {
            pendingstake.modify(source, _self, [&](tempstake & row) {
                row.quantity += quantity;
            });
        } else {
            pendingstake.emplace(_self, [&](tempstake & row) {
                row.sender = from;
                row.quantity = quantity;
                row.memo = memo;
            });
        }
    }
}
