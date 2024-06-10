/*
 * GS1 Syntax Engine example iOS app
 *
 * Copyright (c) 2022-2024 GS1 AISBL.
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
 * This class is a very lightweight shim around the native library,
 * therefore the Swift interface is described here in terms of the public
 * API functions of the native library that each method invokes.
 *
 */

import SwiftUI
import AVFoundation
import MLImage
import MLKit

struct BarcodeScannerView: UIViewRepresentable {

    var updatefn: (_ data: String) -> Void

    func makeUIView(context: UIViewRepresentableContext<BarcodeScannerView>) -> BarcodeScannerUIView {
        BarcodeScannerUIView()
    }

    func updateUIView(_ uiView: BarcodeScannerUIView, context: UIViewRepresentableContext<BarcodeScannerView>) {
        uiView.updatefn = updatefn
    }

}

class BarcodeScannerUIView: UIView, AVCaptureVideoDataOutputSampleBufferDelegate {

    private var captureSession: AVCaptureSession?

    var updatefn: ((_ data: String) -> Void)?

    init() {

        super.init(frame: .zero)

        var allowedAccess = false
        let blocker = DispatchGroup()
        blocker.enter()
        AVCaptureDevice.requestAccess(for: .video) { flag in
            allowedAccess = flag
            blocker.leave()
        }
        blocker.wait()

        if !allowedAccess {
            print("No permission to access the camera")
            return
        }

        let session = AVCaptureSession()
        session.beginConfiguration()
        session.sessionPreset = AVCaptureSession.Preset.medium

        let videoDevice = AVCaptureDevice.default(.builtInWideAngleCamera,
                                                  for: .video, position: .unspecified)
        guard videoDevice != nil, let videoDeviceInput = try? AVCaptureDeviceInput(device: videoDevice!), session.canAddInput(videoDeviceInput) else {
            print("No suitable camera devices detected")
            return
        }
        session.addInput(videoDeviceInput)
        session.commitConfiguration()
        self.captureSession = session

        let videoDataOutput = AVCaptureVideoDataOutput()
        videoDataOutput.videoSettings = [kCVPixelBufferPixelFormatTypeKey : NSNumber(value: kCVPixelFormatType_32BGRA)] as [String : Any]
        videoDataOutput.alwaysDiscardsLateVideoFrames = true

        videoDataOutput.setSampleBufferDelegate(self, queue: DispatchQueue.global(qos: DispatchQoS.QoSClass.default))
        guard captureSession!.canAddOutput(videoDataOutput) else {
            fatalError()
        }
        captureSession!.addOutput(videoDataOutput)

    }

    override class var layerClass: AnyClass {
        AVCaptureVideoPreviewLayer.self
    }

    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    var videoPreviewLayer: AVCaptureVideoPreviewLayer {
        return layer as! AVCaptureVideoPreviewLayer
    }

    override func didMoveToSuperview() {
        super.didMoveToSuperview()
        if nil != self.superview {
            self.videoPreviewLayer.session = self.captureSession
            self.videoPreviewLayer.videoGravity = .resizeAspectFill
            self.videoPreviewLayer.connection?.videoOrientation = .portrait
            DispatchQueue.global(qos: .userInitiated).async { //[weak self] in
                self.captureSession?.startRunning()
            }
        }
        else {
            DispatchQueue.global(qos: .userInitiated).async { //[weak self] in
                self.captureSession?.stopRunning()
            }
        }
    }

    func captureOutput(_ output: AVCaptureOutput, didOutput sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {

        guard CMSampleBufferGetImageBuffer(sampleBuffer) != nil else {
            print("Failed to read image from sample buffer.")
            return
        }

        let image = VisionImage(buffer: sampleBuffer)

        let barcodeOptions = BarcodeScannerOptions(formats: [BarcodeFormat.EAN13, BarcodeFormat.EAN8, BarcodeFormat.UPCA, BarcodeFormat.UPCE, BarcodeFormat.code128, BarcodeFormat.dataMatrix, BarcodeFormat.qrCode])
        let barcodeScanner = BarcodeScanner.barcodeScanner(options: barcodeOptions)

        var barcodes: [Barcode] = []
        var scanningError: Error?
        do {
            barcodes = try barcodeScanner.results(in: image)
        } catch let error {
            scanningError = error
        }

        DispatchQueue.main.sync {
            if let scanningError = scanningError {
                print("Scan error: \(scanningError.localizedDescription).")
                return
            }
            guard !barcodes.isEmpty else {
                return
            }

            /*
             *  We do our best to harmonise the result of scanning, but the output of
             *  ML Kit leaves a lot to be desired in terms of consistency!
             *
             */

            let raw = barcodes.first!.rawValue ?? ""

            var inputData: String

            switch (barcodes.first!.format) {

                // Good: GS1 symbols are returned with AIM symbol identifier ]C1
            case BarcodeFormat.code128:
                if (raw.hasPrefix("]C1")) {
                    inputData = raw
                } else {
                    inputData = "Non-GS1 Code 128, without FNC1 in first!"
                }

                // Questionable: Returned without AIM symbology identifiers
            case BarcodeFormat.EAN13, BarcodeFormat.UPCA, BarcodeFormat.UPCE:
                inputData = "]E0" + raw

                // Questionable: Returned without AIM symbology identifiers
            case BarcodeFormat.EAN8:
                inputData = "]E4" + raw

                // Bad: Symbols are returned without AIM symbology identifiers, but
                // for GS1 symbols we at least have a proxy in the form of GS in
                // first position!
            case BarcodeFormat.dataMatrix:
                if (raw.hasPrefix("http://") || raw.hasPrefix("HTTP://") || raw.hasPrefix("https://") || raw.hasPrefix("HTTPS://")) {
                    inputData = "]d1" + raw
                } else if (raw.hasPrefix("\u{001d}")) {
                    inputData = "]d2" + raw.dropFirst()
                } else {
                    inputData = "Non-GS1 Data Matrix, without FNC1 in first!"
                }

                // Really bad: Returned without AIM symbology identifiers, and
                // worst of all for GS1 symbols there isn't even a proxy since there
                // is no reported GS in first position
            case BarcodeFormat.qrCode:
                if (raw.hasPrefix("http://") || raw.hasPrefix("HTTP://") || raw.hasPrefix("https://") || raw.hasPrefix("HTTPS://")) {
                    inputData = "]Q1" + raw
                } else {  // Best we can do is to assume GS1 format
                    inputData = "]Q3" + raw
                }

            default:
                inputData  = "Unrecognised format"
            }

            updatefn?(inputData)

        }

    }

}
