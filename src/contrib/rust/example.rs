/*
 * GS1 Barcode Syntax Engine
 *
 * This is a contributed example that shows how a Rust language binding might
 * be developed.
 *
 *
 * @author Copyright (c) 2021-2026 GS1 AISBL.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

use gs1encoders::{GS1Encoder, Symbology, Validation};

fn main() {
    let gs1encoder = GS1Encoder::new().unwrap_or_else(|error| panic!("{}", error));

    println!("\nVersion: {}\n", gs1encoder.get_version());

    // Set symbology to DataMatrix
    gs1encoder
        .set_sym(Symbology::Dm)
        .unwrap_or_else(|error| panic!("{}", error));
    println!("Symbology: {:?}", gs1encoder.get_sym());

    // Demonstrate validation control
    println!(
        "RequisiteAis validation enabled: {}",
        gs1encoder.get_validation_enabled(Validation::RequisiteAis)
    );
    gs1encoder
        .set_validation_enabled(Validation::RequisiteAis, false)
        .unwrap();
    println!(
        "RequisiteAis validation enabled (after disable): {}",
        gs1encoder.get_validation_enabled(Validation::RequisiteAis)
    );

    // Set AI data
    let ai_data = "(01)09521234543213(10)ABC123(99)TEST";
    gs1encoder
        .set_ai_data_str(ai_data)
        .unwrap_or_else(|error| panic!("{}", error));
    println!("\nAI IN:  {}", ai_data);
    println!(
        "AI OUT: {}",
        gs1encoder
            .get_ai_data_str()
            .unwrap_or("Not AI data".to_string())
    );
    println!("DATA:   {}", gs1encoder.get_data_str());

    // HRI with data titles
    gs1encoder.set_include_data_titles_in_hri(true).unwrap();
    println!(
        "\nHRI{}:",
        if gs1encoder.get_include_data_titles_in_hri() {
            " (including data titles)"
        } else {
            ""
        }
    );
    for h in gs1encoder.get_hri() {
        println!("    {}", h);
    }

    // DL URI
    match gs1encoder.get_dl_uri(Some("https://example.com")) {
        Some(uri) => println!("\nDL URI: {}", uri),
        Option::None => println!("\nDL URI: Failed: {}", gs1encoder.get_err_markup()),
    }

    // Scan data round-trip
    match gs1encoder.get_scan_data() {
        Some(scan_data) => println!("Scan data: {}", scan_data),
        None => println!("Scan data: None"),
    }

    // DL ignored query params
    let ignored = gs1encoder.get_dl_ignored_query_params();
    if !ignored.is_empty() {
        println!("\nIgnored DL query params:");
        for p in &ignored {
            println!("    {}", p);
        }
    }

    // Demonstrate boolean properties
    println!("\nAdd check digit: {}", gs1encoder.get_add_check_digit());
    println!(
        "Permit unknown AIs: {}",
        gs1encoder.get_permit_unknown_ais()
    );
    println!(
        "Permit zero-suppressed GTIN in DL URIs: {}",
        gs1encoder.get_permit_zero_suppressed_gtin_in_dl_uris()
    );
}
