#include "escrow.hpp"

escrow::~escrow() {}

[[eosio::on_notify("eosio.token::transfer")]]
void escrow::transfer( const name     from,
                       const name     to,
                       const asset    quantity,
                       const string   memo )
{

    if (to != _self) {
        return;
    }

    require_auth( from );

    auto by_sender = escrows.get_index<"bysender"_n>();

    uint8_t found = 0;

    for (auto esc_itr = by_sender.lower_bound(from.value), end_itr = by_sender.upper_bound(from.value); esc_itr != end_itr; ++esc_itr) {
        if (esc_itr->ext_asset.quantity.amount == 0){

            by_sender.modify(esc_itr, from, [&](auto & row) {
                row.ext_asset = extended_asset{quantity, sending_code};
            });

            found = 1;

            break;
        }
    }

    check(found, "Could not find existing escrow to deposit to, transfer cancelled");
}

ACTION escrow::init( const name           sender,
                     const name           receiver,
                     const name           approver,
                     const name           escrow_name,
                     const time_point_sec expires_at,
                     const string         memo)
{
    // Validate user input
    check( sender != receiver, "cannot escrow to self" );
    check( receiver != approver, "receiver cannot be approver" );
    require_auth( sender );
    check( is_account( receiver ), "receiver account does not exist");
    check( is_account( approver ), "approver account does not exist");
    check( escrow_name.length() > 2, "escrow name should be at least 3 characters long.");

    // Validate expire time_point_sec
    check(expires_at > current_time_point(), "expires_at must be a value in the future.");
    time_point_sec max_expires_at = current_time_point() + time_point_sec(SIX_MONTHS_IN_SECONDS);
    check(expires_at <= max_expires_at, "expires_at must be within 6 months from now.");

    // Enforce `sender` as BOS Executive & `approver` as EOSIO
    // Asserts should be removed once escrow.bos is ready for public use
    check(sender == name("bet.bos"), "sender must be bet.bos");
    check(approver == name("eosio"), "approver must be eosio");

    // Notify the following accounts
    require_recipient( sender );
    require_recipient( receiver );
    require_recipient( approver );

    // Set Escrow deposit as `eosio.token` BOS (Extended Asset)
    extended_asset zero_asset{{0, symbol{"BOS", 4}}, "eosio.token"_n};


    // Sender can only have one un-filled escrow
    // Sender must either transfer BOS to `escrow.bos` or `cancel` the existing escrow
    auto by_sender = escrows.get_index<"bysender"_n>();
    for (auto esc_itr = by_sender.lower_bound(sender.value), end_itr = by_sender.upper_bound(sender.value); esc_itr != end_itr; ++esc_itr) {
        check(esc_itr->ext_asset.quantity.amount != 0, "You already have an empty escrow.  Either transfer BOS to escrow.bos or cancel the escrow");
    }

    // Escrow name must be unique
    auto esc_itr = escrows.find(escrow_name.value);
    check(esc_itr == escrows.end(), "escrow with same name already exists.");

    // Update `escrows` table
    escrows.emplace(sender, [&](auto & row) {
        row.escrow_name = escrow_name;
        row.sender = sender;
        row.receiver = receiver;
        row.approver = approver;
        row.ext_asset = zero_asset;
        row.expires_at = expires_at;
        row.created_at = current_time_point();
        row.memo = memo;
        row.locked = false;
    });
}

ACTION escrow::approve( const name escrow_name, const name approver )
{
    require_auth( approver );

    // Check if `escrow_name` already exists
    auto esc_itr = escrows.find(escrow_name.value);
    check(esc_itr != escrows.end(), "Could not find escrow with that name");

    // Cannot approve escrow with 0 BOS deposits
    check(esc_itr->ext_asset.quantity.amount > 0, "This has not been initialized with a transfer");

    // Only `sender` or `approver` can approve escrow
    check(esc_itr->sender == approver || esc_itr->approver == approver, "You are not allowed to approve this escrow.");

    // Must not already be approved
    auto approvals = esc_itr->approvals;
    check(std::find(approvals.begin(), approvals.end(), approver) == approvals.end(), "You have already approved this escrow");

    // Update `escrows` table
    escrows.modify(esc_itr, approver, [&](auto & row){
        // if approver is bet.bos, no change, allow proposer to claim 100% of the fund
        // if approver is BPs, only keep 90% fund for proposer to claim, and BET.BOS will manually execute transfer ACTION in escrow.bos to send fund to each BPs and each auditors
        if (approver == name("eosio")) {
            row.ext_asset.quantity.amount = row.ext_asset.quantity.amount * 0.90;
        }
        row.approvals.push_back(approver);
    });
}

ACTION escrow::unapprove( const name escrow_name, const name disapprover )
{
    require_auth( disapprover );

    // Check if `escrow_name` already exists
    auto esc_itr = escrows.find(escrow_name.value);
    check(esc_itr != escrows.end(), "Could not find escrow with that name");

    // Update `escrows` table
    escrows.modify(esc_itr, name{0}, [&](auto & row) {
        auto existing = std::find(row.approvals.begin(), row.approvals.end(), disapprover);
        check(existing != row.approvals.end(), "You have NOT approved this escrow");
        row.approvals.erase(existing);
    });
}

ACTION escrow::claim( const name escrow_name )
{
    // Check if `escrow_name` already exists
    auto esc_itr = escrows.find(escrow_name.value);
    check(esc_itr != escrows.end(), "Could not find escrow with that name");

    // Escrow must initialized (escrow must be filled with BOS)
    check(esc_itr->ext_asset.quantity.amount > 0, "This has not been initialized with a transfer");

    // Check if escrow is locked by `approver`
    check(esc_itr->locked == false, "This escrow has been locked by the approver");

    // Check if escrow has been approved by `approver` or `sender`
    auto approvals = esc_itr->approvals;
    check(approvals.size() >= 1, "This escrow has not received the required approvals to claim");

    // Transfer escrow funds from `escrow.bos` to `receiver`
    eosio::action(
            eosio::permission_level{_self , "active"_n }, // escrow.bos@active
            esc_itr->ext_asset.contract, // eosio.token
            "transfer"_n,
            make_tuple(
                _self, // from (escrow.bos)
                esc_itr->receiver, // to (sender)
                esc_itr->ext_asset.quantity, // quantity (BOS quanity from escrow)
                esc_itr->memo // memo (escrow memo from `init`)
            )
    ).send();

    // Remove `escrow_name` from `escrows` table
    escrows.erase(esc_itr);
}

/**
 * Empties an unfilled escrow request
 */
ACTION escrow::cancel(const name escrow_name)
{
    // Check if `escrow_name` already exists
    auto esc_itr = escrows.find(escrow_name.value);
    check(esc_itr != escrows.end(), "Could not find escrow with that name");

    // Only `sender` can `cancel` an unfilled escrow
    require_auth(esc_itr->sender);

    // Can only cancel escrow which contains 0 BOS
    check(0 == esc_itr->ext_asset.quantity.amount, "Amount is not zero, this escrow is locked down");

    // Remove `escrow_name` from `escrows` table
    escrows.erase(esc_itr);
}

/**
 * Allows the sender to withdraw the funds if there are not enough approvals and the escrow has expired
 */
ACTION escrow::refund(const name escrow_name)
{
    // Check if `escrow_name` already exists
    auto esc_itr = escrows.find(escrow_name.value);
    check(esc_itr != escrows.end(), "Could not find escrow with that name");

    // Only `sender` can `refund` an expired escrow
    require_auth(esc_itr->sender);

    // Escrow must contain BOS tokens
    check(esc_itr->ext_asset.quantity.amount > 0, "This has not been initialized with a transfer");

    // Escrow cannot be locked by `approver`
    check(esc_itr->locked == false, "This escrow has been locked by the approver");

    // Check if escrow is expired
    time_point_sec time_now = time_point_sec(current_time_point());
    check(time_now >= esc_itr->expires_at, "Escrow has not expired");

    // Transfer back escrow funds from `escrow.bos` to `sender`
    eosio::action(
            eosio::permission_level{_self , "active"_n }, // escrow.bos@active
            esc_itr->ext_asset.contract, // eosio.token
            "transfer"_n,
            make_tuple(
                _self, // from (escrow.bos)
                esc_itr->sender, // to (sender)
                esc_itr->ext_asset.quantity, // quantity (BOS quanity from escrow)
                esc_itr->memo // memo (TO-DO add custom refund message)
            )
    ).send();

    // Remove `escrow_name` from `escrows` table
    escrows.erase(esc_itr);
}

/**
 * Allows the sender to extend the expiry
 */
ACTION escrow::extend(const name escrow_name, const time_point_sec expires_at)
{
    // Check if `escrow_name` already exists
    auto esc_itr = escrows.find(escrow_name.value);
    check(esc_itr != escrows.end(), "Could not find escrow with that name");

    // Escrow must be initialized (transfer BOS to escrow.bos)
    check(esc_itr->ext_asset.quantity.amount > 0, "This has not been initialized with a transfer");

    time_point_sec time_now = time_point_sec(current_time_point());

    // `approver` may extend or shorten the time
    // `sender` may only extend
    if ( has_auth( esc_itr->sender ) ) {
        check(expires_at > esc_itr->expires_at, "You may only extend the expiry");
    } else {
        require_auth( esc_itr->approver );
    }

    // Modify `escrows` table with new `expire_at` value
    escrows.modify(esc_itr, eosio::same_payer, [&](auto & row){
        row.expires_at = expires_at;
    });
}

/**
 * Allows the `approver` to close and refund an unexpired escrow
 */
ACTION escrow::close(const name escrow_name)
{
    // Check if `escrow_name` already exists
    auto esc_itr = escrows.find(escrow_name.value);
    check(esc_itr != escrows.end(), "Could not find escrow with that name");

    // Only `approver` can `close` escrow
    require_auth(esc_itr->approver);

    // Escrow must be initialized (transfer BOS to escrow.bos)
    check(esc_itr->ext_asset.quantity.amount > 0, "This has not been initialized with a transfer");

    // Transfer back escrow funds from `escrow.bos` to `sender`
    eosio::action(
            eosio::permission_level{_self , "active"_n }, // escrow.bos@active
            esc_itr->ext_asset.contract, // eosio.token
            "transfer"_n,
            make_tuple(
                _self, // from (escrow.bos)
                esc_itr->sender, // to (sender)
                esc_itr->ext_asset.quantity, // quantity (BOS quanity from escrow)
                esc_itr->memo // memo (TO-DO add custom close message)
            )
    ).send();

    // Remove `escrow_name` from `escrows` table
    escrows.erase(esc_itr);
}

/**
 * Allows the `approver` to lock an escrow preventing any actions by `sender` or `receiver`.
 */
ACTION escrow::lock(const name escrow_name, const bool locked)
{
    // Check if `escrow_name` already exists
    auto esc_itr = escrows.find(escrow_name.value);
    check(esc_itr != escrows.end(), "Could not find escrow with that name");

    // Only `approver` can lock escrow
    require_auth(esc_itr->approver);

    // Escrow must be initialized (transfer BOS to escrow.bos)
    check(esc_itr->ext_asset.quantity.amount > 0, "This has not been initialized with a transfer");

    // Modify `escrows` table with lock boolean (true/false)
    escrows.modify(esc_itr, eosio::same_payer, [&](auto & row) {
        row.locked = locked;
    });
}

ACTION escrow::clean()
{
    // Only `escrow.bos` can call `clean` action
    require_auth(_self);

    // Remove all rows from `escrows` table
    auto itr = escrows.begin();
    while (itr != escrows.end()){
        itr = escrows.erase(itr);
    }
}
