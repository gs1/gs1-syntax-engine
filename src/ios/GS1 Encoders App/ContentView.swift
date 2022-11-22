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

    @State var libver :String
    @State var inputData :String = ""
    @State var errorMsg :String = ""
    @State var errorIsHidden :Bool = true
    @State var infoMsg :String = ""
    @State var infoIsHidden :Bool = true
    @State var syntax :String = ""
    @State var dataStr :String = ""
    @State var aiDataStr :String = ""
    @State var dlURI :String = ""
    @State var hri :String = ""
    
    @State var unknownAIs = false
    @State var associations = false
    @State var datatitles = false
    
    @FocusState var inputIsFocused: Bool
    
    let gs1encoder = try! GS1Encoder()
    
    init() {
        
        UIScrollView.appearance().bounces = false
        
        inputData = "https://example.com/01/12312312312333/10/ABC123?99=TESTING"
        libver = "GS1 Encoders library: " + gs1encoder.getVersion()
  
    }
    
    func clearRender() {
        syntax = ""
        dataStr = ""
        aiDataStr = ""
        dlURI = ""
        hri = ""
        infoMsg = ""
        infoIsHidden = true
        errorMsg = ""
        errorIsHidden = true
    }
    
    func loadDataValues() {
        unknownAIs = gs1encoder.getPermitUnknownAIs()
        associations = gs1encoder.getValidateAIassociations()
        datatitles = gs1encoder.getIncludeDataTitlesInHRI()

        if (gs1encoder.getDataStr() == "") {
            return
        }
        
        dataStr = gs1encoder.getDataStr()
 
        let ai = gs1encoder.getAIdataStr()
        aiDataStr = ai != "" ? ai : "⧚ Not AI-based data ⧛"
        
        do {
            dlURI = try gs1encoder.getDLuri()
        } catch GS1EncoderError.digitalLinkError(let msg) {
            dlURI = msg
        } catch {}
         
        hri = gs1encoder.getHRI().joined(separator: "\n")
        
        let qps = gs1encoder.getDLignoredQueryParams()
        if (qps.count > 0) {
            infoMsg = "Warning: Non-numeric query parameters ignored: ⧚" +
                       qps.joined(separator: "&") + "⧚"
            infoIsHidden = false
       }
    }
    
    func processInput() {
        
        clearRender()
        
        let data = inputData.trimmingCharacters(in: .whitespacesAndNewlines)
        if (data == "") {
            return
        }
        
        do {
            
            if (data.starts(with: "(")) {
                syntax = "Bracketed AI element string"
                try gs1encoder.setAIdataStr(data)
            } else if (data.starts(with: "]")) {
                syntax = "Barcode scan data with AIM symbology identifier"
                try gs1encoder.setScanData(data.replacingOccurrences(of: "{GS}", with: "\u{001d}"))
            } else if (data.starts(with: "^")) {
                syntax = "Unbracketed AI element string"
                try gs1encoder.setDataStr(data)
            } else if (data.starts(with: "http://") || data.starts(with: "https://")) {
                syntax = "Digital Link URI"
                try gs1encoder.setDataStr(data)
            } else if (CharacterSet(charactersIn: data).isSubset(of: CharacterSet(charactersIn: "0123456789"))) {
                syntax =  "Plain data"
                
                if (data.count != 8 && data.count != 12 && data.count != 13 && data.count != 14) {
                    errorMsg = "Invalid length for a GTIN-8, GTIN-12, GTIN-13 or GTIN-14"
                    errorIsHidden = false
                    return
                }
                
                // Perform a checksum validation here, since the Syntax Engine validates only AI-based data
                var parity = 0
                var weight = data.count % 2 == 0 ? 3 : 1
                for d in data.dropLast() {
                    parity += weight * d.wholeNumberValue!
                    weight = 4 - weight
                }
                parity = (10 - parity % 10) % 10
                
                if (parity != data.last!.wholeNumberValue!) {
                    errorMsg = "Incorrect numeric check digit"
                    errorIsHidden = false
                    infoMsg = "Plain data validation failed: " + data.dropLast() + "⧚" + String(data.last!) + "⧛"
                    infoIsHidden = false
                    return
                }
                
                syntax = "Plain GTIN-" + String(data.count) + " - converted to AI (01)..."
                try gs1encoder.setDataStr("^01" + String(repeating: "0", count: 14 - data.count) + data)
                
            } else {
                syntax =  "Non-numeric plain data is not a valid GS1 syntax"
                return
            }
            
        } catch GS1EncoderError.parameterError(let msg),
                GS1EncoderError.scanDataError(let msg),
                GS1EncoderError.digitalLinkError(let msg) {
            errorMsg = msg
            errorIsHidden = false
            let markup = gs1encoder.getErrMarkup()
            if (markup != "") {
                infoMsg = "AI content validation failed: " + markup.replacingOccurrences(of: "|", with: "⧚")
                infoIsHidden = false
            }
            return
        } catch {
            errorMsg = "Unknown error"
            errorIsHidden = false
            return
        }

        inputIsFocused = false
        
        loadDataValues()
        
    }
    
    var body: some View {
        ScrollView() {
            VStack {
                TextField("", text: $libver).frame(maxWidth: .infinity).padding().background(Color.blue).font(.title2).foregroundColor(.white)
                HStack {
                    Toggle("Permit unknown AIs", isOn: $unknownAIs).toggleStyle(CheckboxToggleStyle()).onChange(of: unknownAIs) { value in
                        clearRender()
                        try! gs1encoder.setPermitUnknownAIs(value)
                    }
                    Toggle("Validate AI associations", isOn: $associations).toggleStyle(CheckboxToggleStyle()).onChange(of: associations) { value in
                        clearRender()
                        try! gs1encoder.setValidateAIassociations(value)
                    }
                    Toggle("Include data titles in HRI", isOn: $datatitles).toggleStyle(CheckboxToggleStyle()).onChange(of: datatitles) { value in
                        clearRender()
                        try! gs1encoder.setIncludeDataTitlesInHRI(value)
                    }
                }
                
                TextFieldWithBorder(label: "Input data", message: $inputData).focused($inputIsFocused).onChange(of: inputData) { value in
                    clearRender()
                }
                
                Button {
                    processInput()
                } label: {
                    Text("PROCESS INPUT").frame(maxWidth: .infinity)
                }
                .buttonStyle(.borderedProminent)
                
                OutputView(errorMsg: $errorMsg, errorIsHidden: $errorIsHidden, infoMsg: $infoMsg, infoIsHidden: $infoIsHidden, syntax: $syntax, dataStr: $dataStr, aiDataStr: $aiDataStr, dlURI: $dlURI, hri: $hri)
                
                Spacer()
            }.onAppear(perform: loadDataValues)
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}

struct CheckboxToggleStyle: ToggleStyle {
    func makeBody(configuration: Configuration) -> some View {
        return HStack {
            Image(systemName: configuration.isOn ? "checkmark.square" : "square")
                .resizable()
                .frame(width: 22, height: 22)
                .onTapGesture { configuration.isOn.toggle() }
            configuration.label.font(.caption)
        }
    }
}

struct TextFieldWithBorder: View {
    var label :String
    @Binding var message :String
    
    var body: some View {
        ZStack(alignment: .topLeading) {
            TextField("", text: $message, axis: .vertical)
                .autocorrectionDisabled(true)
                .textInputAutocapitalization(.never)
                .offset(x:0, y:6).lineLimit(2...6)
                .font(.body).textFieldStyle(.roundedBorder).padding(.bottom, 8)
            HStack {
                Text(label).font(.caption).foregroundColor(Color.blue).background(.white).offset(x: -10, y: -20)
                Spacer()
            }.padding()
        }
    }
}

struct OutputView: View {
    
    @Binding var errorMsg :String
    @Binding var errorIsHidden :Bool
    @Binding var infoMsg :String
    @Binding var infoIsHidden :Bool
    @Binding var syntax :String
    @Binding var dataStr :String
    @Binding var aiDataStr :String
    @Binding var dlURI :String
    @Binding var hri :String
    
    var body: some View {
        if (!errorIsHidden) {
            TextFieldWithBorder(label: "Error", message: $errorMsg).disabled(true)
        }
        if (!infoIsHidden) {
            TextFieldWithBorder(label: "Info", message: $infoMsg).disabled(true)
        }
        TextFieldWithBorder(label: "Detected syntax", message: $syntax).disabled(true)
        TextFieldWithBorder(label: "Barcode message (^ = FNC1)", message: $dataStr).disabled(true)
        TextFieldWithBorder(label: "GS1 AI element string", message: $aiDataStr).disabled(true)
        TextFieldWithBorder(label: "GS1 Digital Link URI (canonical form)", message: $dlURI).disabled(true)
        TextFieldWithBorder(label: "HRI", message: $hri).disabled(true)
    }
}
