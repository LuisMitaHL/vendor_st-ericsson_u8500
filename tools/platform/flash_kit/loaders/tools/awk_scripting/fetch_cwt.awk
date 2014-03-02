{ match($0, /Sprint_([0-9]+)_T(([0-9\\.]+)|(NPI)?)/, arr); printf "export cwt=%s", arr[2] }
