/*
 * GS1 Syntax Engine example Android app using the Java binding
 *
 * @author Copyright (c) 2022-2023 GS1 AISBL.
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

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.text.Editable
import android.text.TextWatcher
import android.view.View
import android.view.inputmethod.InputMethodManager
import androidx.activity.result.contract.ActivityResultContracts
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

    override fun onDestroy() {
        super.onDestroy()
        gs1encoder.free()
    }

    private fun clearRender() {
        binding.syntaxTextBox.setText("")
        binding.datastrTextBox.setText("")
        binding.elementstringTextBox.setText("")
        binding.dlTextBox.setText("")
        binding.hriTextBox.setText("")

        binding.infoTextBox.setText("")
        binding.infoTextBox.visibility = View.GONE
        binding.infoTextInputLayout.visibility = View.GONE

        binding.errmsgTextBox.setText("")
        binding.errmsgTextBox.visibility = View.GONE
        binding.errmsgTextInputLayout.visibility = View.GONE
    }

    private fun loadDataValues() {

        binding.unknownAIsCheckBox.isChecked = gs1encoder.permitUnknownAIs
        binding.associationsCheckBox.isChecked = gs1encoder.getValidationEnabled(GS1Encoder.Validation.RequisiteAIs);
        binding.dataTitlesCheckBox.isChecked = gs1encoder.includeDataTitlesInHRI

        if (gs1encoder.dataStr == "")
            return

        binding.datastrTextBox.setText(gs1encoder.dataStr)

        val ai = gs1encoder.aIdataStr ?: "⧚ Not AI-based data ⧛"
        binding.elementstringTextBox.setText(ai)

        try {
            binding.dlTextBox.setText(gs1encoder.getDLuri(null))
        } catch (e: GS1EncoderDigitalLinkException) {
            binding.dlTextBox.setText(e.message)
        }

        val hri = gs1encoder.hri
        if (hri.isNotEmpty()) {
            binding.hriTextBox.setText(hri.joinToString("\n"))
        } else {
            binding.hriTextBox.setText("⧚ Not AI-based data ⧛")
        }

        val qps = gs1encoder.dLignoredQueryParams
        if (qps.isNotEmpty()) {
            binding.infoTextBox.setText(
                "Warning: Non-numeric query parameters ignored: ⧚" +
                        qps.joinToString("&") + "⧚"
            )
            binding.infoTextBox.visibility = View.VISIBLE
            binding.infoTextInputLayout.visibility = View.VISIBLE
        }

    }

    fun processInputButtonClicked(@Suppress("UNUSED_PARAMETER") view: View) {

        clearRender()

        val data = binding.inputData.text.toString()
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
                binding.syntaxTextBox.setText("GS1 Digital Link URI")
                gs1encoder.dataStr = data
            } else if (data.matches("^\\d+$".toRegex())) {
                binding.syntaxTextBox.setText("Plain data")

                if (data.length != 8 && data.length != 12 && data.length != 13 && data.length != 14) {
                        binding.errmsgTextBox.setText("Invalid length for a GTIN-8, GTIN-12, GTIN-13 or GTIN-14")
                        binding.errmsgTextBox.visibility = View.VISIBLE
                        binding.errmsgTextInputLayout.visibility = View.VISIBLE
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
                        binding.errmsgTextBox.visibility = View.VISIBLE
                        binding.errmsgTextInputLayout.visibility = View.VISIBLE

                        binding.infoTextBox.setText("Plain data validation failed: " + data.dropLast(1) + "⧚" + data.last() + "⧛")
                        binding.infoTextBox.visibility = View.VISIBLE
                        binding.infoTextInputLayout.visibility = View.VISIBLE
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
                    binding.errmsgTextBox.visibility = View.VISIBLE
                    binding.errmsgTextInputLayout.visibility = View.VISIBLE

                    val markup = gs1encoder.errMarkup
                    if (markup != "") {
                        binding.infoTextBox.setText(
                            "AI content validation failed: " + markup.replace("|","⧚")
                        )
                        binding.infoTextBox.visibility = View.VISIBLE
                        binding.infoTextInputLayout.visibility = View.VISIBLE
                    }
                    return
                }
                else -> throw e
            }
        }

        // Release the input field and hide the keyboard
        binding.inputData.clearFocus()
        val imm: InputMethodManager = getSystemService(INPUT_METHOD_SERVICE) as InputMethodManager
        imm.hideSoftInputFromWindow(window.decorView.windowToken, 0)

        loadDataValues()

    }

    private val scanInputDataLauncher =
        registerForActivityResult(
            ActivityResultContracts.StartActivityForResult()
        ) {
            if (it.resultCode != Activity.RESULT_OK) {
                binding.inputData.setText("Scan failed")
                return@registerForActivityResult
            }
            val value = it.data?.getStringExtra("INPUT_DATA") ?: ""
            binding.inputData.setText(value.replace("\u001D", "{GS}"))
        }

    fun scanBarcodeButtonClicked(@Suppress("UNUSED_PARAMETER") view: View) {
        clearRender()
        val intent = Intent(this, GS1BarcodeScannerActivity::class.java)
        scanInputDataLauncher.launch(intent)
    }

    fun unknownAIsCheckBoxClicked(@Suppress("UNUSED_PARAMETER") view: View) {
        clearRender()
        gs1encoder.permitUnknownAIs = binding.unknownAIsCheckBox.isChecked
    }

    fun associationsCheckBoxClicked(@Suppress("UNUSED_PARAMETER") view: View) {
        clearRender()
        gs1encoder.setValidationEnabled(GS1Encoder.Validation.RequisiteAIs, binding.associationsCheckBox.isChecked);
    }

    fun dataTitlesCheckBoxClicked(@Suppress("UNUSED_PARAMETER") view: View) {
        clearRender()
        gs1encoder.includeDataTitlesInHRI = binding.dataTitlesCheckBox.isChecked
    }

}
