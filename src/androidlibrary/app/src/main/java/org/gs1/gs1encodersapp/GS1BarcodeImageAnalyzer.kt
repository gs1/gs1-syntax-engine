/*
 * GS1 Syntax Engine example Android app using the Java binding
 *
 * @author Copyright (c) 2022 GS1 AISBL.
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

package org.gs1.gs1encodersapp

import android.annotation.SuppressLint
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import com.google.mlkit.vision.barcode.BarcodeScannerOptions
import com.google.mlkit.vision.barcode.BarcodeScanning
import com.google.mlkit.vision.barcode.common.Barcode
import com.google.mlkit.vision.common.InputImage

class GS1BarcodeImageAnalyzer(private val result: (String) -> Unit) : ImageAnalysis.Analyzer {
    @SuppressLint("UnsafeOptInUsageError")
    override fun analyze(imageProxy: ImageProxy) {
        val image = imageProxy.image ?: return
        val inputImage = InputImage.fromMediaImage(image, imageProxy.imageInfo.rotationDegrees)
        val options = BarcodeScannerOptions.Builder()
            .setBarcodeFormats(
                Barcode.FORMAT_CODE_128,
                Barcode.FORMAT_EAN_13,
                Barcode.FORMAT_EAN_8,
                Barcode.FORMAT_UPC_A,
                Barcode.FORMAT_UPC_E,
                Barcode.FORMAT_QR_CODE,
                Barcode.FORMAT_DATA_MATRIX)
            .build()
        val scanner = BarcodeScanning.getClient(options)
        scanner.process(inputImage)
            .addOnSuccessListener { barcodes ->
                // Return the first scanned barcode
                if (barcodes.isNotEmpty()) {
                    with(barcodes.first()) {

                        /*
                         *  We do our best to harmonise the result of scanning, but the output of
                         *  ML Kit leaves a lot to be desired in terms of consistency!
                         *
                         */

                        val raw = this.rawValue ?: ""
                        var inputData: String

                        when (this.format) {

                            // Good: GS1 symbols are returned with AIM symbol identifier ]C1
                            Barcode.FORMAT_CODE_128 -> {
                                inputData = if (raw.startsWith("]C1")) {
                                    raw
                                } else {
                                    "Non-GS1 Code 128, without FNC1 in first!"
                                }
                            }

                            // Questionable: Returned without AIM symbology identifiers
                            Barcode.FORMAT_EAN_13, Barcode.FORMAT_UPC_A, Barcode.FORMAT_UPC_E -> {
                                inputData = "]E0$raw"
                            }

                            // Questionable: Returned without AIM symbology identifiers
                            Barcode.FORMAT_EAN_8 -> {
                                inputData = "]E4$raw"
                            }

                            // Bad: Symbols are returned without AIM symbology identifiers, but
                            // for GS1 symbols we at least have a proxy in the form of GS in
                            // first position!
                            Barcode.FORMAT_DATA_MATRIX -> {
                                inputData = if (raw.startsWith("http://") or raw.startsWith("https://")) {
                                    "]d1$raw"
                                } else if (raw.startsWith("\u001D")) {
                                    "]d2" + raw.drop(1)
                                } else {
                                    "Non-GS1 Data Matrix, without FNC1 in first!"
                                }
                            }

                            // Really bad: Returned without AIM symbology identifiers, and
                            // worst of all for GS1 symbols there isn't even a proxy since there
                            // is no reported GS in first position
                            Barcode.FORMAT_QR_CODE -> {
                                inputData = if (raw.startsWith("http://") or raw.startsWith("https://")) {
                                    "]Q1$raw"
                                } else {  // Best we can do is to assume GS1 format
                                    "]Q3$raw"
                                }
                            }

                            else -> {
                                inputData = "Unrecognised format"
                            }

                        }

                        result.invoke(inputData)

                    }
                }
                imageProxy.close()
            }
            .addOnFailureListener {
                imageProxy.close()
            }
            .addOnCompleteListener {
                imageProxy.close()
            }

    }
}