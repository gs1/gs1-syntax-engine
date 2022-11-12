/*
 * GS1 Encoders App for Swift
 *
 * Copyright (c) 2022 GS1 AISBL.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

import SwiftUI

struct ContentView: View {
    
    var text :String = "";
    
    init() {
        
        let in_text = "^011231231231232699ABC"
        
        do {
            
            let gs1encoder = try GS1Encoder()
            text.append("Library version: " + gs1encoder.getVersion() + "\n\n")
            
            try gs1encoder.setDataStr(in_text)
            text.append("IN: " + in_text + "\n\n")
            
            text.append("AI: " + gs1encoder.getAIdataStr() + "\n\n")
            
            let dl_text = try gs1encoder.getDLuri("https://example.org/")
            text.append("DL: " + dl_text + "\n\n")
            
            try gs1encoder.setIncludeDataTitlesInHRI(true)
            text.append("HRI (with data titles):\n\n" + gs1encoder.getHRI().joined(separator: "\n"))
        
        } catch GS1EncoderError.generalError(let msg) {
            text = msg
        } catch GS1EncoderError.parameterError(let msg) {
            text = msg
        } catch GS1EncoderError.digitalLinkError(let msg) {
            text = msg
        } catch GS1EncoderError.scanDataError(let msg) {
            text = msg
        } catch {
            text = "Unexpected error \(error)"
        }
            
    }
        
    var body: some View {
        VStack {
            Text(text)
        }
        .padding()
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
