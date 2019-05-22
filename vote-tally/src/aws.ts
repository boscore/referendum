import * as AWS from "aws-sdk";
import { AWS_BUCKET, AWS_ACCESS_KEY_ID, AWS_SECRET_ACCESS_KEY, AWS_REGION } from "./config";

export function uploadS3(filepath: string, data: any) {
    const s3 = new AWS.S3({
        accessKeyId: AWS_ACCESS_KEY_ID,
        secretAccessKey: AWS_SECRET_ACCESS_KEY,
        region: AWS_REGION,
    });
    const Bucket = AWS_BUCKET;
    const Key = filepath;

    const params = {
        Bucket,
        Key,
        Body: JSON.stringify(data, null, 4),
        ACL: "public-read",
        ContentType: "JSON",
    };

    s3.putObject(params, (err) => {
        if (err) console.error(`[ERROR] aws.pubObject ${err.message}`);
        else console.log(`aws.pubObject => s3://${Bucket}/${Key}`);
    });
}