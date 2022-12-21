/*
 * GS1 Syntax Engine
 *
 * This is a contributed example that shows how a Rust language binding might
 * be developed.
 *
 *
 * @author Copyright (c) 2021-2022 GS1 AISBL.
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

use gs1encoders::GS1Encoder;

fn main() {

    let data_str = "^010952123454321310ABC123^99TEST";
    let data_titles = true;

    let gs1encoder = GS1Encoder::new().unwrap_or_else(|error| {
        panic!("{}", error)
    });

    println!("\nVersion: {}\n", gs1encoder.get_version());

    gs1encoder.set_data_str(data_str).unwrap_or_else(|error| {
        panic!("{}", error)
    });

    println!("IN: {}", gs1encoder.get_data_str());
    println!("AI: {}", gs1encoder.get_ai_data_str().unwrap_or("Not AI data".to_string()));

    gs1encoder.set_include_data_titles_in_hri(data_titles).unwrap();
    println!("\nHRI{}:", if gs1encoder.get_include_data_titles_in_hri() { " (including data titles)" } else { "" });

    for h in gs1encoder.get_hri() {
        println!("    {}", h);
    }

}
