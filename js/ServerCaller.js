class ServerCaller {
    constructor(serverURL, sessionId) {
        this._serverURL = serverURL;
        this._sessionId = sessionId;
    }

    _encodeHTMLForm (data)
    {
        let params = new Array(0);

        for (let name in data ) {
            const value = data[name];
            const param = encodeURIComponent(name) + '=' + encodeURIComponent(value);

            params.push( param );
        }

        return params.join("&").replace( /%20/g, "+");
    }

    CallServerPost(command, params = {}, retType = null) {
        return new Promise((resolve, reject) => {
            // Add session ID in params
            const encParams = this._encodeHTMLForm(params);

            let xhr = new XMLHttpRequest();
            xhr.open("POST", this._serverURL + "/" + command + "?session_id=" + this._sessionId, true);
            xhr.setRequestHeader( 'Content-Type', 'application/x-www-form-urlencoded' );
            if ("INT" == retType || "FLOAT" == retType) {
                xhr.responseType = "arraybuffer";
            }
            xhr.onreadystatechange = () => {
                if(xhr.readyState === XMLHttpRequest.DONE && xhr.status === 200) {
                    console.log(xhr.statusText);
                    switch (retType) {
                        case "INT": {
                            let arr = new Int32Array(xhr.response);
                            if (arr.length) resolve(arr);
                            else reject();
                        } break;
                        case "FLOAT": {
                            const arr = new Float32Array(xhr.response); 
                            if (arr.length) resolve(arr);
                            else reject();
                        } break;
                        default: {
                            if ("OK" == xhr.statusText) resolve();
                            else reject();
                        } break;
                    }
                }
            }
            xhr.onerror = () => {
                reject(xhr.statusText);
            };

            xhr.send(encParams);
        });
    }

    CallServerSubmitFile(formData) {
        return new Promise((resolve, reject) => {
            const xhr = new XMLHttpRequest();
            xhr.responseType = "arraybuffer";

            xhr.onload = (e) => {
                console.log(xhr.statusText);
                const arr = new Float32Array(xhr.response); 
                if (arr.length) resolve(arr);
                else reject();
            };

            xhr.open("POST" , this._serverURL + "/FileUpload?session_id=" + this._sessionId);

            xhr.send(formData);
        });
    }
}