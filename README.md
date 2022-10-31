# wizfilocallizer
WizFiLoCaLLizer - Your Personal Localizer &amp; a Direct Hotline

Triggered by the recent corona pandemic where many companies now implement full or hybrid working-from-home and policy, I've built WizFiLoCaLLizer for my office-desk to let my colleagues know where I am at any given time and let them reach me directly with a click-of-a-button. It's been operational on my office-desk for several months now and my colleagues really do love it!

The beauty is that, as this is personalized gadget, you have full control of what, where and when the localization occurs e.g. you're ok to tell your colleagues if you're at home but perhaps you're not comfortable to tell them if you're at your parents home. The trigger conditions and locations are all in your control and all is executed automatically in real-time.

Furthermore WizFiLoCaLLizer allows people to reach your private phone number without disclosing your number in the first place. Very handy if you have separate work and private phone number that you want to keep it to yourself as much as possible.

WizFiLoCaLLizer relies on IFTTT internet service to trigger an action based on the info and the status in your smartphone. For example if you are connected to your home Wi-Fi network then it automatically triggers an action informing that you are currently at home or if you're connected to your car Bluetooth system, it then knows you are on the road. Surely, based on outdoor GPS location you can also defined certain area that you want people to know through the WizFiLoCaLLizer. IFTTT is huge and the limit is only your imagination!

Relaying the information that IFTTT provides, Blynk cloud service is used that bridge the information from the cloud to the WizFiLoCaLLizer device via WiZfi360 Wi-Fi connectivity. Blynk also provide beautiful Web-UI and smartphone apps interface to interact with WizFiLoCaLLizer, such that you can do much more thing like changing the scrolling text speed or sending a custom message.

On top of that, thanks to SIM800L module that is on board, WizFiLoCaLLizer has all the bells-and-whistles to enable a phone call as soon as you press the RED button. The loud-speaker and the microphone are embedded on the unit and ready to go.

At last I've built the WizFiLoCaLLizer in such a way that it is easy to duplicate so that you can build it yourself! I've only used THT components on purpose so you can make it seamlessly even on the breadboard. I've prepared the code in Arduino as well and compile it into in a single .ino file to ease-up your SW installation. More importantly, the online services I've prescribed here are all free (up to certain limit of course). Only sufficient credit for a making a phone call in your sim card is needed :)
