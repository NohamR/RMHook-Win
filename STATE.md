Without hook

[*] Loaded config: host=rm.noh.am, port=443
[*] Initializing MinHook
[+] Qt and MQTT DLLs loaded
[+] Resolved ?createRequest@QNetworkAccessManager@@MEAAPEAVQNetworkReply@@W4Operation@1@AEBVQNetworkRequest@@PEAVQIODevice@@@Z at 140718018555616
[+] Resolved ?open@QWebSocket@@QEAAXAEBVQNetworkRequest@@@Z at 140718986708784
[+] Hooked createRequest
[+] Hooked QWebSocket::open
[+] Hooks enabled


(4d8.23e4): Access violation - code c0000005 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
ucrtbase!#strlen:
00007ffb`f6a9d040 38400402 ldrb        w2,[x0],#0


[0x0]   ucrtbase!#strlen   0x885a3fb920   0x7ffbf6a9d00c   
[0x1]   ucrtbase!#strlen_entry_thunk+0xc   0x885a3fb920   0x7ffb784faab6   
[0x2]   paho_mqttpp3!mqtt::exception::error_str+0x56   0x885a3fb930   0x7ffb784f5bad   
[0x3]   paho_mqttpp3!mqtt::exception::exception+0x2d   0x885a3fb9d0   0x7ffb784f54ef   
[0x4]   paho_mqttpp3!mqtt::async_client::async_client+0x29f   0x885a3fba40   0x7ffb784f582d   
[0x5]   paho_mqttpp3!mqtt::async_client::async_client+0x5d   0x885a3fbb10   0x7ff764ce0100   
[0x6]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xf1fcf   0x885a3fbba0   0x7ff764cdb066   
[0x7]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xecf35   0x885a3fbcd0   0x7ff764c6a51a   
[0x8]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0x7c3e9   0x885a3fbd10   0x7ff764ca14f5   
[0x9]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xb33c4   0x885a3fbd50   0x7ff764c90318   
[0xa]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xa21e7   0x885a3fbe30   0x7ff764cbcaac   
[0xb]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xce97b   0x885a3fbe90   0x7ff764cbbf0a   
[0xc]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xcddd9   0x885a3fbf60   0x7ffb2a0f1a51   
[0xd]   Qt6Core!QObject::qt_static_metacall+0x1451   0x885a3fbfe0   0x7ffb2a0f4564   
[0xe]   Qt6Core!QMetaObject::activate+0x84   0x885a3fc110   0x7ff764c99964   
[0xf]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xab833   0x885a3fc140   0x7ffb2a0fda9b   
[0x10]   Qt6Core!QMetaCallEvent::placeMetaCall+0x3b   0x885a3fc170   0x7ffb2a0fb8d2   
[0x11]   Qt6Core!QObject::event+0x182   0x885a3fc1b0   0x7ffb2a0b301b   
[0x12]   Qt6Core!QCoreApplication::notify+0xbb   0x885a3fc2c0   0x7ffb2a0b313f   
[0x13]   Qt6Core!QCoreApplication::notifyInternal2+0x10f   0x885a3fc310   0x7ffb2a0b584c   
[0x14]   Qt6Core!QCoreApplicationPrivate::sendPostedEvents+0x1fc   0x885a3fc380   0x7ffb2a23eb70   
[0x15]   Qt6Core!QEventDispatcherWin32::processEvents+0x90   0x885a3fc470   0x7ffb2a0b9c44   
[0x16]   Qt6Core!QEventLoop::exec+0x1c4   0x885a3ff5d0   0x7ffb2a19d90f   
[0x17]   Qt6Core!QThread::exec+0x16f   0x885a3ff670   0x7ffb2a245e29   
[0x18]   Qt6Core!QThread::start+0x579   0x885a3ff6e0   0x7ffbfa0809fc   
[0x19]   KERNEL32!$iexit_thunk$cdecl$i8$i8+0x1c   0x885a3ff780   0x7ffbfa017bb0   
[0x1a]   KERNEL32!#BaseThreadInitThunk+0x30   0x885a3ff7b0   0x7ffbfb67c4c8   
[0x1b]   ntdll!#RtlUserThreadStart+0x48   0x885a3ff7c0   0x0   




With MQTTAsync_createWithOptions from paho-mqtt3as_orig.dll hook to return patched url

[*] Loaded config: host=rm.noh.am, port=443
[*] Initializing MinHook
[+] Qt and MQTT DLLs loaded
[+] Resolved ?createRequest@QNetworkAccessManager@@MEAAPEAVQNetworkReply@@W4Operation@1@AEBVQNetworkRequest@@PEAVQIODevice@@@Z at 140718030679776
[+] Resolved ?open@QWebSocket@@QEAAXAEBVQNetworkRequest@@@Z at 140718986643248
[+] Resolved MQTTAsync_createWithOptions at 140718859938896
[+] Hooked MQTTAsync_createWithOptions
[+] Hooked createRequest
[+] Hooked QWebSocket::open
[+] Hooks enabled



(1184.1c88): Access violation - code c0000005 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
ucrtbase!#strlen:
00007ffb`f6a9d040 38400402 ldrb        w2,[x0],#0


[0x0]   ucrtbase!#strlen   0x70df7fc050   0x7ffbf6a9d00c   
[0x1]   ucrtbase!#strlen_entry_thunk+0xc   0x70df7fc050   0x7ffb7b1daab6   
[0x2]   paho_mqttpp3!mqtt::exception::error_str+0x56   0x70df7fc060   0x7ffb7b1d5bad   
[0x3]   paho_mqttpp3!mqtt::exception::exception+0x2d   0x70df7fc100   0x7ffb7b1d54ef   
[0x4]   paho_mqttpp3!mqtt::async_client::async_client+0x29f   0x70df7fc170   0x7ffb7b1d582d   
[0x5]   paho_mqttpp3!mqtt::async_client::async_client+0x5d   0x70df7fc240   0x7ff764ce0100   
[0x6]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xf1fcf   0x70df7fc2d0   0x7ff764cdb066   
[0x7]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xecf35   0x70df7fc400   0x7ff764c6a51a   
[0x8]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0x7c3e9   0x70df7fc440   0x7ff764ca14f5   
[0x9]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xb33c4   0x70df7fc480   0x7ff764c90318   
[0xa]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xa21e7   0x70df7fc560   0x7ff764cbcaac   
[0xb]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xce97b   0x70df7fc5c0   0x7ff764cbbf0a   
[0xc]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xcddd9   0x70df7fc690   0x7ffb29b81a51   
[0xd]   Qt6Core!QObject::qt_static_metacall+0x1451   0x70df7fc710   0x7ffb29b84564   
[0xe]   Qt6Core!QMetaObject::activate+0x84   0x70df7fc840   0x7ff764c99964   
[0xf]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xab833   0x70df7fc870   0x7ffb29b8da9b   
[0x10]   Qt6Core!QMetaCallEvent::placeMetaCall+0x3b   0x70df7fc8a0   0x7ffb29b8b8d2   
[0x11]   Qt6Core!QObject::event+0x182   0x70df7fc8e0   0x7ffb29b4301b   
[0x12]   Qt6Core!QCoreApplication::notify+0xbb   0x70df7fc9f0   0x7ffb29b4313f   
[0x13]   Qt6Core!QCoreApplication::notifyInternal2+0x10f   0x70df7fca40   0x7ffb29b4584c   
[0x14]   Qt6Core!QCoreApplicationPrivate::sendPostedEvents+0x1fc   0x70df7fcab0   0x7ffb29cceb70   
[0x15]   Qt6Core!QEventDispatcherWin32::processEvents+0x90   0x70df7fcba0   0x7ffb29b49c44   
[0x16]   Qt6Core!QEventLoop::exec+0x1c4   0x70df7ffd00   0x7ffb29c2d90f   
[0x17]   Qt6Core!QThread::exec+0x16f   0x70df7ffda0   0x7ffb29cd5e29   
[0x18]   Qt6Core!QThread::start+0x579   0x70df7ffe10   0x7ffbfa0809fc   
[0x19]   KERNEL32!$iexit_thunk$cdecl$i8$i8+0x1c   0x70df7ffeb0   0x7ffbfa017bb0   
[0x1a]   KERNEL32!#BaseThreadInitThunk+0x30   0x70df7ffee0   0x7ffbfb67c4c8   
[0x1b]   ntdll!#RtlUserThreadStart+0x48   0x70df7ffef0   0x0   


With MQTTAsync_createWithOptions from paho-mqtt3as.dll hook to return patched url

[*] Loaded config: host=rm.noh.am, port=443
[*] Initializing MinHook
[+] Qt and MQTT DLLs loaded
[+] Resolved ?createRequest@QNetworkAccessManager@@MEAAPEAVQNetworkReply@@W4Operation@1@AEBVQNetworkRequest@@PEAVQIODevice@@@Z at 140718064627424
[+] Resolved ?open@QWebSocket@@QEAAXAEBVQNetworkRequest@@@Z at 140719255209776
[+] Resolved MQTTAsync_createWithOptions at 140719258086256
[+] Hooked MQTTAsync_createWithOptions
[+] Hooked createRequest
[+] Hooked QWebSocket::open
[+] Hooks enabled
[MQTT] MQTTAsync_createWithOptions URI: ssl://vernemq-prod.cloud.remarkable.engineering:443 ClientId: noham-ccd226f9-4f37-4c45-a6ee-22f21f5e7548 PersistenceType: 1
[MQTT] originalMQTTAsyncCreate returned -1050472512 Final URI: ssl://rm.noh.am:443


(538.14bc): Access violation - code c0000005 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
ucrtbase!#strlen:
00007ffb`f6a9d040 38400402 ldrb        w2,[x0],#0


[0x0]   ucrtbase!#strlen   0x86d71fbcb0   0x7ffbf6a9d00c   
[0x1]   ucrtbase!#strlen_entry_thunk+0xc   0x86d71fbcb0   0x7ffbc1649348   
[0x2]   paho_mqtt3as!std::_Narrow_char_traits<char,int>::length+0x8   (Inline Function)   (Inline Function)   
[0x3]   paho_mqtt3as!std::basic_string<char,std::char_traits<char>,std::allocator<char> >::{ctor}+0x14   (Inline Function)   (Inline Function)   
[0x4]   paho_mqtt3as!hookedMQTTAsyncCreate+0x88   0x86d71fbcc0   0x7ffba6a1aa95   
[0x5]   paho_mqttpp3!mqtt::exception::error_str+0x35   0x86d71fbe80   0x7ffba6a15bad   
[0x6]   paho_mqttpp3!mqtt::exception::exception+0x2d   0x86d71fbf20   0x7ffba6a154ef   
[0x7]   paho_mqttpp3!mqtt::async_client::async_client+0x29f   0x86d71fbf90   0x7ffba6a1582d   
[0x8]   paho_mqttpp3!mqtt::async_client::async_client+0x5d   0x86d71fc060   0x7ff764ce0100   
[0x9]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xf1fcf   0x86d71fc0f0   0x7ff764cdb066   
[0xa]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xecf35   0x86d71fc220   0x7ff764c6a51a   
[0xb]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0x7c3e9   0x86d71fc260   0x7ff764ca14f5   
[0xc]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xb33c4   0x86d71fc2a0   0x7ff764c90318   
[0xd]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xa21e7   0x86d71fc380   0x7ff764cbcaac   
[0xe]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xce97b   0x86d71fc3e0   0x7ff764cbbf0a   
[0xf]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xcddd9   0x86d71fc4b0   0x7ffb29271a51   
[0x10]   Qt6Core!QObject::qt_static_metacall+0x1451   0x86d71fc530   0x7ffb29274564   
[0x11]   Qt6Core!QMetaObject::activate+0x84   0x86d71fc660   0x7ff764c99964   
[0x12]   reMarkable!rtc::Description::Entry::ExtMap::ExtMap+0xab833   0x86d71fc690   0x7ffb2927da9b   
[0x13]   Qt6Core!QMetaCallEvent::placeMetaCall+0x3b   0x86d71fc6c0   0x7ffb2927b8d2   
[0x14]   Qt6Core!QObject::event+0x182   0x86d71fc700   0x7ffb2923301b   
[0x15]   Qt6Core!QCoreApplication::notify+0xbb   0x86d71fc810   0x7ffb2923313f   
[0x16]   Qt6Core!QCoreApplication::notifyInternal2+0x10f   0x86d71fc860   0x7ffb2923584c   
[0x17]   Qt6Core!QCoreApplicationPrivate::sendPostedEvents+0x1fc   0x86d71fc8d0   0x7ffb293beb70   
[0x18]   Qt6Core!QEventDispatcherWin32::processEvents+0x90   0x86d71fc9c0   0x7ffb29239c44   
[0x19]   Qt6Core!QEventLoop::exec+0x1c4   0x86d71ffb20   0x7ffb2931d90f   
[0x1a]   Qt6Core!QThread::exec+0x16f   0x86d71ffbc0   0x7ffb293c5e29   
[0x1b]   Qt6Core!QThread::start+0x579   0x86d71ffc30   0x7ffbfa0809fc   
[0x1c]   KERNEL32!$iexit_thunk$cdecl$i8$i8+0x1c   0x86d71ffcd0   0x7ffbfa017bb0   
[0x1d]   KERNEL32!#BaseThreadInitThunk+0x30   0x86d71ffd00   0x7ffbfb67c4c8   
[0x1e]   ntdll!#RtlUserThreadStart+0x48   0x86d71ffd10   0x0   
