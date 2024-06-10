#
#  GS1 Syntax Engine
#
#  This is a contributed example that shows how a Python 3 language binding might
#  be developed.
#
#
#  @author Copyright (c) 2021-2024 GS1 AISBL.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

from gs1encoders import GS1Encoder

def main():

    data_str = "^010952123454321310ABC123^99TEST"
    data_titles = True

    gs1encoder = GS1Encoder()

    print("\nVersion: {}\n".format(gs1encoder.get_version()))

    gs1encoder.set_data_str(data_str)

    print("IN: {}".format(gs1encoder.get_data_str()))
    print("AI: {}".format(gs1encoder.get_ai_data_str() or "Not AI data"))

    gs1encoder.set_include_data_titles_in_hri(data_titles)
    print("\nHRI{}:".format(" (including data titles)" if data_titles else ""))

    for h in gs1encoder.get_hri():
      print("    {}".format(h))


if __name__ == "__main__":
    main()
