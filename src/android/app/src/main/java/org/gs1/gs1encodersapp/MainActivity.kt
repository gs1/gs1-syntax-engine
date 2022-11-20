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

import android.os.Bundle
import android.text.Editable
import android.text.TextWatcher
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import org.gs1.gs1encoders.GS1Encoder
import org.gs1.gs1encoders.GS1EncoderDigitalLinkException
import org.gs1.gs1encoders.GS1EncoderParameterException
import org.gs1.gs1encoders.GS1EncoderScanDataException
import org.gs1.gs1encodersapp.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private lateinit var gs1encoder: GS1Encoder

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Must add this listener ourselves because there is no corresponding binding
        binding.inputData.addTextChangedListener(object : TextWatcher {
            override fun afterTextChanged(s: Editable) {}
            override fun beforeTextChanged(s: CharSequence, start: Int, count: Int, after: Int) {}
            override fun onTextChanged(p0: CharSequence?, p1: Int, p2: Int, p3: Int) {
                clearRender()
            }
        })

        // Load the Syntax Engine
        gs1encoder = GS1Encoder()

        (this as? AppCompatActivity)?.supportActionBar?.title = "GS1 Encoders library: " + gs1encoder.version
        binding.inputData.setText("https://example.com/01/12312312312333/10/ABC123?99=TESTING")

        loadDataValues()
    }

    private fun clearRender() {
        binding.syntaxTextBox.setText("")
        binding.datastrTextBox.setText("")
        binding.elementstringTextBox.setText("")
        binding.dlTextBox.setText("")
        binding.hriTextBox.setText("")
        binding.infoTextBox.setText("")
        binding.errmsgTextBox.setText("")
    }

    private fun loadDataValues() {

        binding.unknownAIsCheckBox.isChecked = gs1encoder.permitUnknownAIs
        binding.associationsCheckBox.isChecked = gs1encoder.validateAIassociations
        binding.dataTitlesCheckBox.isChecked = gs1encoder.includeDataTitlesInHRI

        if (gs1encoder.dataStr == "")
            return

        binding.datastrTextBox.setText(gs1encoder.dataStr)
        val ai = gs1encoder.aIdataStr
        if (ai != "")
            binding.elementstringTextBox.setText(gs1encoder.aIdataStr)
        else
            binding.elementstringTextBox.setText("⧚ Not AI-based data ⧛")

        try {
            binding.dlTextBox.setText(gs1encoder.getDLuri(null))
        } catch (e: GS1EncoderDigitalLinkException) {
            binding.dlTextBox.setText(e.message)
        }

        binding.hriTextBox.setText(gs1encoder.hri.joinToString("\n"))

        val qps = gs1encoder.dLignoredQueryParams
        if (qps.isNotEmpty())
                binding.infoTextBox.setText("Warning: Non-numeric query parameters ignored: ⧚" +
                        qps.joinToString("&") + "⧚")

    }

    fun processInputButtonClicked(@Suppress("UNUSED_PARAMETER") view: View) {

        clearRender()

        val data = binding.inputData.text.toString().trim()
        if (data == "")
            return

        try {

            if (data.startsWith('(')) {
                binding.syntaxTextBox.setText("Bracketed AI element string")
                gs1encoder.aIdataStr = data
            } else if (data.startsWith(']')) {
                binding.syntaxTextBox.setText("Barcode scan data with AIM symbology identifier")
                gs1encoder.scanData = data.replace("{GS}", "\u001d")
            } else if (data.startsWith('^')) {
                binding.syntaxTextBox.setText("Unbracketed AI element string")
                gs1encoder.dataStr = data
            } else if (data.startsWith("http://") || data.startsWith("https://")) {
                binding.syntaxTextBox.setText("Digital Link URI")
                gs1encoder.dataStr = data
            } else if (data.matches("^\\d+$".toRegex())) {
                binding.syntaxTextBox.setText("Plain data")

                if (data.length != 8 && data.length != 12 && data.length != 13 && data.length != 14) {
                        binding.errmsgTextBox.setText("Invalid length for a GTIN-8, GTIN-12, GTIN-13 or GTIN-14")
                        return
                }

                // Perform a checksum validation here, since the Syntax Engine validates only AI-based data
                var parity = 0
                var weight: Int = if (data.length % 2 == 0) 3 else 1
                for (d in data.dropLast(1)) {
                        parity += weight * d.digitToInt()
                        weight = 4 - weight
                }
                parity = (10 - parity % 10) % 10

                if (parity != data.last().digitToInt()) {
                        binding.errmsgTextBox.setText("Incorrect numeric check digit")
                        binding.infoTextBox.setText("Plain data validation failed: " + data.dropLast(1) + "⧚" + data.last() + "⧛")
                        return
                }

                binding.syntaxTextBox.setText("Plain GTIN-" + data.length + " - converted to AI (01)...")
                gs1encoder.dataStr = "^01" + data.padStart(14, '0')
            } else {
                binding.syntaxTextBox.setText("Non-numeric plain data is not a valid GS1 syntax")
                return
            }

        } catch (e: Exception) {
            when (e) {
                is GS1EncoderParameterException,
                is GS1EncoderScanDataException,
                is GS1EncoderDigitalLinkException -> {
                    binding.errmsgTextBox.setText("Error: " + e.message)
                    val markup = gs1encoder.errMarkup
                    if (markup != "")
                        binding.infoTextBox.setText(
                            "AI content validation failed: " + markup.replace("|","⧚")
                        )
                    return
                }
                else -> throw e
            }
        }

        loadDataValues()

    }

    fun unknownAIsCheckBoxClicked(@Suppress("UNUSED_PARAMETER") view: View) {
        clearRender()
        gs1encoder.permitUnknownAIs = binding.unknownAIsCheckBox.isChecked
    }

    fun associationsCheckBoxClicked(@Suppress("UNUSED_PARAMETER") view: View) {
        clearRender()
        gs1encoder.validateAIassociations = binding.associationsCheckBox.isChecked
    }

    fun dataTitlesCheckBoxClicked(@Suppress("UNUSED_PARAMETER") view: View) {
        clearRender()
        gs1encoder.includeDataTitlesInHRI = binding.dataTitlesCheckBox.isChecked
    }

}
