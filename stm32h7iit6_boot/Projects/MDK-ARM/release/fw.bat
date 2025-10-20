.\release\S2v1.4.exe .\release\boot.mot "DR10DS BT" .\release\DR10DS_boot.mot
.\release\app.exe gen .\release\DR10DS_boot.mot boot2 none .\release\DR10DS_boot.ds
.\release\app.exe conv .\release\DR10DS_boot.mot boot2 none 0x3C0000 .\release\DR10DS_boot.bin
