/*
 * GS1 Syntax Engine example user of the Java binding
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

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import org.gs1.gs1encodersapp.databinding.ActivityMainBinding
import org.gs1.gs1encoders.GS1Encoder
import org.gs1.gs1encoders.GS1EncoderParameterException

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private lateinit var gs1encoder: GS1Encoder;

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)


        val intext = "^011231231231232699ABC"


        gs1encoder = GS1Encoder()

        try {
            gs1encoder.dataStr = intext
        } catch (e: GS1EncoderParameterException) {
            binding.sampleText.text = e.message
            return
        }

        val estext = gs1encoder.aIdataStr;
        val dluri = gs1encoder.getDLuri("https://example.org/")

        gs1encoder.includeDataTitlesInHRI = true
        val hritext = gs1encoder.hri.joinToString("\n\n")

        binding.sampleText.text =
            "IN: $intext\n\n\nAI: $estext\n\n\nDL: $dluri\n\n\nHRI (with data titles):\n\n$hritext"

    }

}
