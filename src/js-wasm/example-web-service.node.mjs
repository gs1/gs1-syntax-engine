/*
 *  Basic Node.js example web service that uses the WASM or JS-only build of
 *  the GS1 Barcode Syntax Engine.
 *
 *  Requirements:
 *
 *    - WASM or JS-only build of the Syntax Engine in the current directory or
 *      the gs1encoder package installed via npm
 *
 *    - Any maintained version of Node.js
 *
 *  Copyright (c) 2022-2025 GS1 AISBL.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *
 *  Command line arguments:
 *
 *    /verbose                  Enable verbose debug logging.
 *    /logfile=<filename>       Write logs to file instead of stdout.
 *    /bind=<address>           Bind address (default: 127.0.0.1). Use 0.0.0.0 for all interfaces.
 *    /port=<number>            Port number (default: 3030).
 *
 *    /(un)installservice       Attempt to install/uninstall this example as a
 *                              Windows service. Any other arguments (e.g.
 *                              /verbose, /logfile=<filename>, /bind=<address>,
 *                              /port=<number>) are passed to the service.
 *                              See "Service installation on Windows" section
 *                              near the end of this file for requirements.
 *
 *  Example startup with verbose logging as terminal output and to a file:
 *
 *    $ node example-web-service.node.mjs /verbose
 *    $ node example-web-service.node.mjs /verbose /logfile=service.log
 *    $ node example-web-service.node.mjs /bind=0.0.0.0 /port=8080
 *
 *  Output format:
 *
 *    By default, responses are in plaintext format. JSON output can be requested
 *    either by adding the query parameter "output=json" or by setting the HTTP
 *    header "Accept: application/json". Error responses will also be returned as
 *    JSON when the Accept header is set.
 *
 *  Query parameters:
 *
 *    input                               Required. The barcode data to process.
 *    output=json                         Request JSON output format, irrespective of Accept: header.
 *    includeDataTitlesInHRI              Include AI titles in human-readable output.
 *    permitUnknownAIs                    Allow processing of unknown AIs.
 *    permitZeroSuppressedGTINinDLuris    Allow zero-suppressed GTINs in Digital Link URIs.
 *    noValidateRequisiteAIs              Disable validation of requisite AIs.
 *
 *  Endpoints:
 *
 *    /dataStr      Process input as a barcode message or Digital Link URI.
 *    /aiDataStr    Process input as an AI element string.
 *    /scanData     Process input as raw scan data.
 *
 *  Example calls:
 *
 *    $ curl -i 'http://127.0.0.1:3030/aiDataStr?input=(02)12312312312319(99)ASDFEE&includeDataTitlesInHRI'
 *
 *    HTTP/1.1 422 Bad Request
 *    Content-Type: text/plain
 *    ...
 *    Required AIs for AI (02) are not satisfied: 37
 *
 *
 *    $ curl -i 'http://127.0.0.1:3030/aiDataStr?input=(02)12312312312319(99)ASDFEE&includeDataTitlesInHRI&noValidateRequisiteAIs'
 *    HTTP/1.1 200 OK
 *    Content-Type: text/plain
 *    ...
 *    Barcode message:      ^021231231231231999ASDFEE
 *    AI element string:    (02)12312312312319(99)ASDFEE
 *    GS1 Digital Link URI: ⧚ Cannot create a DL URI without a primary key AI ⧚
 *    HRI:
 *           CONTENT (02) 12312312312319
 *           INTERNAL (99) ASDFEE
 *
 *    $ curl -i 'http://127.0.0.1:3030/dataStr?input=HTTPS://ID.EXAMPLE.ORG/01/12312312312319?99=ASDFEE&output=json'
 *    HTTP/1.1 200 OK
 *    Content-Type: application/json
 *    ...
 *    {"dataStr":"http://ID.EXAMPLE.ORG/01/12312312312319?99=ASDFEE","aiDataStr":"(01)12312312312319(99)ASDFEE","dlURI":"https://id.gs1.org/01/12312312312319?99=ASDFEE","hri":["(01) 12312312312319","(99) ASDFEE"]}
 *
 *    $ curl -i 'http://127.0.0.1:3030/scanData?input=%5Dd2011231231231231910ABC123%1D21SERIAL456'
 *    HTTP/1.1 200 OK
 *    Content-Type: text/plain
 *    ...
 *    Barcode message:      ^011231231231231910ABC123^21SERIAL456
 *    AI element string:    (01)12312312312319(10)ABC123(21)SERIAL456
 *    GS1 Digital Link URI: https://id.gs1.org/01/12312312312319/10/ABC123/21/SERIAL456
 *    HRI:
 *           (01) 12312312312319
 *           (10) ABC123
 *           (21) SERIAL456
 *
 */

"use strict";


/*
 *  ------ User-defined tunables ------
 *
 */
let bind = '127.0.0.1';  // "0.0.0.0" to allow remote connections
let port = 3030;


/*
 *  ------ Argument processing -------
 *
 */
import { argv } from 'process';

// Defer to service processing
if (argv.includes('/installservice') || argv.includes('/uninstallservice')) {
    await handleWindowsService();
    process.exit();
}

// Parse command line arguments
const verbose = argv.some(arg => arg === '/verbose');
let logFile = null;
let logStream = null;

for (const arg of argv) {
    if (arg.startsWith('/logfile=')) {
        logFile = arg.substring(9);
    } else if (arg.startsWith('/bind=')) {
        bind = arg.substring(6);
    } else if (arg.startsWith('/port=')) {
        const portNum = parseInt(arg.substring(6), 10);
        if (isNaN(portNum) || portNum < 1 || portNum > 65535) {
            console.error('Error: Invalid port number. Must be between 1 and 65535.');
            process.exit(1);
        }
        port = portNum;
    }
}

// Setup logging
import { createWriteStream } from 'fs';

if (logFile) {
    try {
        logStream = createWriteStream(logFile, { flags: 'a' });
        logStream.on('error', (err) => {
            console.error('Error writing to log file:', err);
        });
    } catch (err) {
        console.error('Failed to open log file:', err);
        process.exit(1);
    }
}

let lastLogTime = null;
let requestCounter = 0;

function log(message) {
    if (!verbose) return;
    const now = process.hrtime.bigint();
    const timestamp = new Date().toISOString();
    let delta = '';
    if (lastLogTime !== null) {
        const diffNanos = now - lastLogTime;
        const diffSeconds = Number(diffNanos) / 1000000000;
        delta = ` +${diffSeconds.toFixed(6)}s`;
    } else {
        delta = '           ';
    }
    lastLogTime = now;
    const logMessage = `[${timestamp}${delta}] ${message}\n`;
    if (logStream) {
        logStream.write(logMessage);
    } else {
        process.stdout.write(logMessage);
    }
}

function logRequestComplete(requestStartTime) {
    if (!verbose) return;
    const totalNanos = process.hrtime.bigint() - requestStartTime;
    const totalMs = Number(totalNanos) / 1000000;
    log(`  Request completed in ${totalMs.toFixed(3)}ms`);
    log('');
}

function formatErrorResponse(message, acceptHeader, markup = null) {
    const wantsJson = acceptHeader && acceptHeader.includes('application/json');
    if (wantsJson) {
        const errorObj = { error: message };
        if (markup) {
            errorObj.markup = markup.replace(/\|/g, "⧚");
        }
        return {
            contentType: 'application/json',
            body: JSON.stringify(errorObj) + "\n"
        };
    } else {
        let body = message + "\n";
        if (markup) {
            body += markup.replace(/\|/g, "⧚") + "\n";
        }
        return {
            contentType: 'text/plain',
            body: body
        };
    }
}

function sendErrorResponse(res, statusCode, statusText, message, acceptHeader, requestStartTime, markup = null) {
    if (markup) {
        log(`  Error markup: ${markup}`);
    }
    const errorResponse = formatErrorResponse(message, acceptHeader, markup);
    res.writeHead(statusCode, {'Content-Type': errorResponse.contentType});
    log(`  Response: ${statusCode} ${statusText}`);
    res.write(errorResponse.body);
    log(`  Response body: ${errorResponse.body.trimEnd()}`);
    res.end();
    logRequestComplete(requestStartTime);
}

function buildJsonResponse(dataStr, aiDataStr, dlURI, dlURIerr, hri) {
    const response = { dataStr, aiDataStr, dlURI, hri };
    if (dlURIerr) {
        response.dlURIerror = dlURIerr.message;
    }
    return {
        contentType: 'application/json',
        body: JSON.stringify(response) + "\n"
    };
}

function buildPlaintextResponse(dataStr, aiDataStr, dlURI, dlURIerr, hri) {
    const plaintextLines = [];
    plaintextLines.push("Barcode message:      " + dataStr);
    plaintextLines.push("AI element string:    " + (aiDataStr ?? "⧚ Not AI-based data ⧚"));
    plaintextLines.push("GS1 Digital Link URI: " + (dlURI ?? "⧚ " + dlURIerr.message + " ⧚"));
    plaintextLines.push("HRI:                  " + (dataStr !== "" && hri.length === 0 ? "⧚ Not AI-based data ⧚": ""));
    hri.forEach(ai => plaintextLines.push("       " + ai));
    return {
        contentType: 'text/plain',
        body: plaintextLines.join("\n") + "\n"
    };
}

function sendSuccessResponse(res, response, requestStartTime) {
    res.writeHead(200, {'Content-Type': response.contentType});
    log(`  Response: 200 OK (${response.contentType})`);
    res.write(response.body);
    log(`  Response body:\n${response.body.trimEnd()}`);
    res.end();
    logRequestComplete(requestStartTime);
}


/*
 *  ------ Main processing -------
 *
 *  To run this example with an instance of gs1encoder that you have installed
 *  from npm change the following import to:
 *
 *    import { GS1encoder } from "gs1encoder";
 *
 */
import { GS1encoder } from "./gs1encoder.mjs";

const gs1encoder = new GS1encoder();
await gs1encoder.init();

import * as http from 'http';


http.createServer(function(req, res) {

    const requestStartTime = process.hrtime.bigint();
    lastLogTime = null;
    requestCounter++;
    log(`Request #${requestCounter}: ${req.method} ${req.url}`);

    const urlObj = new URL(req.url, `http://${req.headers.host}`);

    const pathname = urlObj.pathname;
    log(`  Pathname: ${pathname}`);

    const params = Object.fromEntries(urlObj.searchParams);
    log(`  Query params: ${JSON.stringify(params)}`);

    if (req.method !== 'GET') {
        sendErrorResponse(res, 405, 'Method Not Allowed', 'Method Not Allowed', req.headers['accept'], requestStartTime);
        return;
    }

    const inpStr = params.input;
    if (!inpStr) {
        sendErrorResponse(res, 400, 'Bad Request', "Bad Request: 'input' query parameter must be defined", req.headers['accept'], requestStartTime);
        return;
    }

    try {

        /*
         *  Example of setting options
         *
         */
        log(`  Setting GS1encoder options:`);

        gs1encoder.includeDataTitlesInHRI = "includeDataTitlesInHRI" in params;
        log(`    includeDataTitlesInHRI: ${gs1encoder.includeDataTitlesInHRI}`);

        gs1encoder.permitUnknownAIs = "permitUnknownAIs" in params;
        log(`    permitUnknownAIs: ${gs1encoder.permitUnknownAIs}`);

        gs1encoder.permitZeroSuppressedGTINinDLuris = "permitZeroSuppressedGTINinDLuris" in params;
        log(`    permitZeroSuppressedGTINinDLuris: ${gs1encoder.permitZeroSuppressedGTINinDLuris}`);

        gs1encoder.setValidationEnabled(GS1encoder.validation.RequisiteAIs, !("noValidateRequisiteAIs" in params));
        log(`    validateRequisiteAIs: ${gs1encoder.getValidationEnabled(GS1encoder.validation.RequisiteAIs)}`);

        /*
         *  Set the data
         *
         */
        switch (pathname) {
            case '/dataStr':
                log(`  Calling gs1encoder.dataStr = "${inpStr}"`);
                gs1encoder.dataStr = inpStr;
                break;
            case '/aiDataStr':
                log(`  Calling gs1encoder.aiDataStr = "${inpStr}"`);
                gs1encoder.aiDataStr = inpStr;
                break;
            case '/scanData':
                log(`  Calling gs1encoder.scanData = "${inpStr}"`);
                gs1encoder.scanData = inpStr;
                break;
            default:
                sendErrorResponse(res, 404, 'Not Found', 'Not Found', req.headers['accept'], requestStartTime);
                return;
        }

    } catch (err) {
        log(`  GS1encoder error: ${err.message}`);
        const markup = gs1encoder.errMarkup;
        sendErrorResponse(res, 422, 'Unprocessable Entity', err.message, req.headers['accept'], requestStartTime, markup);
        return;
    }

    log(`  GS1encoder results:`);

    const dataStr = gs1encoder.dataStr;
    log(`    dataStr: ${dataStr}`);

    const aiDataStr = gs1encoder.aiDataStr;
    log(`    aiDataStr: ${aiDataStr ?? "(null)"}`);

    let dlURI;
    let dlURIerr;
    try {
        dlURI = gs1encoder.getDLuri(null);
    } catch (err) {
        dlURI = null;
        dlURIerr = err;
        log(`  getDLuri error: ${err.message}`);
    }
    log(`    dlURI: ${dlURI ?? "(error)"}`);

    const hri = gs1encoder.hri;
    log(`    hri: ${JSON.stringify(hri)}`);

    // Determine output format from query parameter or Accept header
    const acceptHeader = req.headers['accept'] || '';
    const wantsJson = params.output === 'json' || acceptHeader.includes('application/json');

    const response = wantsJson
        ? buildJsonResponse(dataStr, aiDataStr, dlURI, dlURIerr, hri)
        : buildPlaintextResponse(dataStr, aiDataStr, dlURI, dlURIerr, hri);

    sendSuccessResponse(res, response, requestStartTime);

}).listen(port, bind);

console.log("Web service is running on %s:%d", bind, port);
if (verbose) {
    console.log("Verbose logging enabled" + (logFile ? ` (output to ${logFile})` : " (output to stdout)"));
}


// ----- Service installation on Windows -------

/*
 *  To install as a service on Windows:
 *
 *      npm install -g node-windows
 *      npm link node-windows
 *      node example-web-service.node.mjs /installservice
 *
 *  To install with custom options (bind address, port, logging):
 *
 *      node example-web-service.node.mjs /installservice /bind=0.0.0.0 /port=8080 /verbose /logfile=service.log
 *
 *  All arguments except /installservice and /uninstallservice are passed to the service.
 *
 *  To uninstall the service:
 *
 *      node example-web-service.node.mjs /uninstallservice
 *
 */
async function handleWindowsService() {

    const { Service } = await import('node-windows');
    const path = await import('path');
    const scriptPath = path.resolve(process.argv[1]);

    // Collect script arguments to pass to the service (excluding /installservice and /uninstallservice)
    const scriptArgs = argv.slice(2).filter(arg =>
        arg !== '/installservice' && arg !== '/uninstallservice'
    );

    const svc = new Service({
        name: 'GS1 Barcode Syntax Engine',
        description: 'GS1 Barcode Syntax Engine HTTP API demo service.',
        script: scriptPath,
        scriptOptions: scriptArgs.join(' ')
    });

    if (argv.includes('/installservice')) {

        await new Promise((resolve, reject) => {
            svc.on('install', () => {
                console.log('Service installed.');
                resolve();
            });
            svc.on('alreadyinstalled', () => {
                console.log('Service already installed.');
                resolve();
            });
            svc.on('error', err => {
                console.error('Error installing service:', err);
                reject(err);
            });
            svc.install();
        });

        await new Promise((resolve, reject) => {
            svc.on('start', () => {
                console.log('Service started.');
                resolve();
            });
            svc.on('error', err => {
                console.error('Error starting service:', err);
                reject(err);
            });
            svc.start();
        });

    }

    if (argv.includes('/uninstallservice')) {

        await new Promise((resolve, reject) => {
            svc.on('uninstall', () => {
                console.log('Service uninstalled.');
                resolve();
            });
            svc.on('error', (err) => {
                console.error('Error uninstalling service:', err);
            reject(err);
            });
            svc.uninstall();
        });

    }
}
