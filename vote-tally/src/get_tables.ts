import { delay } from "./utils";
import { rpc, DELAY_MS } from "./config";
import { EosioDelband } from "./interfaces";

/**
 * Get Table `eosio::delband`
 */
export async function get_table_delband(scopes: Set<string>) {
    const delband: EosioDelband[] = [];
    for (const scope of Array.from(scopes)) {
        console.log(`get_table_rows [eosio::${scope}:userres]`);
        const response = await rpc.get_table_rows<EosioDelband>("eosio", scope, "delband", {json: true });

        for (const row of response.rows) {
            // Only include `delband` that is self delegated
            if (row.from == row.to) delband.push(row);
        }
    }
    return delband;
}

/**
 * Get Tables
 */
export async function get_tables<T>(code: string, scope: string, table: string, lower_bound_key: string, delete_keys: string[] = []): Promise<T[]> {
    let lower_bound = "";
    const limit = 1500;
    const rows = new Map<string, T>();

    while (true) {
        console.log(`get_table_rows [${code}::${scope}:${table}] size=${rows.size} lower=${lower_bound}`);
        const response: any = await rpc.get_table_rows<T>(code, scope, table, {
            json: true,
            lower_bound,
            limit,
        });
        for (const row of response.rows) {
            // Delete extra fields
            for (const key of delete_keys) {
                delete row[key];
            }

            // Adding to Map removes duplicates entries
            const key = row[lower_bound_key];
            rows.set(key, row);

            // Set lower bound
            lower_bound = key;
        }
        // prevent hitting rate limits from API endpoints
        await delay(DELAY_MS);

        // end of table rows
        if (response.more === false) break;
    }
    return Array.from(rows.values());
}
