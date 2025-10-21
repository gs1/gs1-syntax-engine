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
 *    /verbose                  Enable verbose debug logging
 *    /logfile=<filename>       Write logs to file instead of stdout
 *
 *    /(un)installservice       Attempt to install/uninstall this example as a
 *                              Windows service. Any other arguments (e.g.
 *                              /verbose, /logfile=<filename>) are passed to
 *                              the service.
 *                              See the additional requirements at the bottom
 *                              of this code.
 *
 *  Example startup with verbose logging as terminal output and to a file:
 *
 *    $ node example-web-service.node.mjs /verbose
 *    $ node example-web-service.node.mjs /verbose /logfile=service.log
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
 */

"use strict";


/*
 *  ------ User-defined tunables ------
 *
 */
const bind = '127.0.0.1';  // "0.0.0.0" to allow remote connections
const port = 3030;


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

// Setup logging
import { createWriteStream } from 'fs';

var verbose = argv.some(arg => arg === '/verbose');
var logFile = null;
var logStream = null;

for (const arg of argv) {
    if (arg.startsWith('/logfile=')) {
        logFile = arg.substring(9);
        break;
    }
}

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

var lastLogTime = null;
var requestCounter = 0;

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

var gs1encoder = new GS1encoder();
await gs1encoder.init();

import * as http from 'http';


http.createServer(function(req, res) {

    const requestStartTime = process.hrtime.bigint();
    lastLogTime = null;
    requestCounter++;
    log(`Request #${requestCounter}: ${req.method} ${req.url}`);

    var urlObj = new URL(req.url, `http://${req.headers.host}`);

    var pathname = urlObj.pathname;
    log(`  Pathname: ${pathname}`);

    var params = Object.fromEntries(urlObj.searchParams);
    log(`  Query params: ${JSON.stringify(params)}`);

    if (req.method !== 'GET') {
        const responseBody = "Method Not Allowed\n";

        res.writeHead(405, {'Content-Type': 'text/plain'});
        log(`  Response: 405 Method Not Allowed`);

        res.write(responseBody);
        log(`  Response body: ${responseBody.trimEnd()}`);

        res.end();
        logRequestComplete(requestStartTime);

        return;
    }

    var inpStr = params.input;
    if (!inpStr) {
        const responseBody = "Bad Request: 'input' query parameter must be defined\n";

        res.writeHead(400, {'Content-Type': 'text/plain'});
        log(`  Response: 400 Bad Request`);

        res.write(responseBody);
        log(`  Response body: ${responseBody.trimEnd()}`);

        res.end();
        logRequestComplete(requestStartTime);

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
            default:
                const responseBody = 'Not Found';
                res.writeHead(404, {'Context-Type': 'text/plain'});
                log(`  Response: 404 Not Found`);
                res.end(responseBody);
                log(`  Response body: ${responseBody}`);
                logRequestComplete(requestStartTime);
                return;
        }

    } catch (err) {
        log(`  GS1encoder error: ${err.message}`);

        var markup = gs1encoder.errMarkup;
        if (markup) {
            log(`  Error markup: ${markup}`);
        }

        var responseBody = err.message + "\n";
        if (markup)
            responseBody += markup.replace(/\|/g, "⧚") + "\n";

        res.writeHead(422, {'Content-Type': 'text/plain'});
        log(`  Response: 422 Unprocessable Entity`);

        res.write(responseBody);
        log(`  Response body: ${responseBody.trimEnd()}`);

        res.end();
        logRequestComplete(requestStartTime);

        return;
    }

    log(`  GS1encoder results:`);

    var dataStr = gs1encoder.dataStr;
    log(`    dataStr: ${dataStr}`);

    var aiDataStr = gs1encoder.aiDataStr;
    log(`    aiDataStr: ${aiDataStr ?? "(null)"}`);

    var dlURI; var dlURIerr;
    try {
        dlURI = gs1encoder.getDLuri(null);
    } catch (err) {
        dlURI = null;
        dlURIerr = err;
        log(`  getDLuri error: ${err.message}`);
    }
    log(`    dlURI: ${dlURI ?? "(error)"}`);

    var hri = gs1encoder.hri;
    log(`    hri: ${JSON.stringify(hri)}`);

    var contentType; var responseBody;

    if (params.output === 'json') {
        contentType = 'application/json';

        var ret = {
            dataStr: dataStr,
            aiDataStr: aiDataStr,
            dlURI: dlURI,
            hri: hri
        }

        responseBody = JSON.stringify(ret) + "\n";
    } else {
        contentType = 'text/plain';

        var plaintextLines = [];
        plaintextLines.push("Barcode message:      " + dataStr);
        plaintextLines.push("AI element string:    " + (aiDataStr ?? "⧚ Not AI-based data ⧚"));
        plaintextLines.push("GS1 Digital Link URI: " + (dlURI ?? "⧚ " + dlURIerr.message + " ⧚"));
        plaintextLines.push("HRI:                  " + (dataStr !== "" && hri.length == 0 ? "⧚ Not AI-based data ⧚": ""));
        hri.forEach(ai => plaintextLines.push("       " + ai));

        responseBody = plaintextLines.join("\n") + "\n";
    }

    res.writeHead(200, {'Content-Type': contentType});
    log(`  Response: 200 OK (${contentType})`);

    res.write(responseBody);
    log(`  Response body:\n${responseBody.trimEnd()}`);

    res.end();
    logRequestComplete(requestStartTime);

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
