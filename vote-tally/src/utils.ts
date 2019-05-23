import * as crypto from 'crypto';

/**
 * Promise Delay
 *
 * @param {number} ms Milisecond delay
 * @return {Promise<void>}
 * @example
 *
 * await delay(100);
 */
export function delay(ms: number) {
  return new Promise((resolve) => {
      setTimeout(() => {
          resolve()
      }, ms);
  })
}

/**
 * Create Hash from JSON object
 *
 * @param {object} json JSON object
 * @returns {string} md5 hash
 * @example
 *
 * createHash({foo: "bar"}) // => 9bb58f26192e4ba00f01e2e7b136bbd8
 */
export function createHash(json: any) {
    const data = JSON.stringify(json);
    return crypto.createHash('md5').update(data).digest("hex");
}

/**
 * Parse Token String
 *
 * @param {string} tokenString Token String (eg: "10.0 EOS")
 * @returns {object} Amount & Symbol
 * @example
 * parseTokenString("10.0 EOS") //=> {amount: 10.0, symbol: "EOS"}
 */
export function parseTokenString(tokenString: string) {
    const [amountString, symbol] = tokenString.split(" ");
    const amount = parseFloat(amountString);
    return {amount, symbol};
}

/**
 * Disjoint
 *
 * Two sets are said to be disjoint sets if they have no element in common.
 */
export function disjoint(source: Set<string>, target: Set<string>) {
    const result = new Set<string>();
    for (const item of Array.from(source)) {
        if (!target.has(item)) result.add(item);
    }
    return result;
}
