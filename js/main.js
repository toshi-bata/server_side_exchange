class Main {
    constructor() {
        this._viewer;
        this._port;
        this._reverseProxy;
        this._sessionId;
        this._serverCaller;
        this._tol = 1e-4;
        this._bodyNodes = {};
        this._units = {
            "1.0": "mm",
            "25.4": "inch",
            "10.0": "cm",
            "1000.0": "m"
        };
        this._roHoles = [];
    }

    start (port, reverseProxy) {
        this._port = port;
        this._reverseProxy = reverseProxy;
        this._requestServerProcess();
        this._createViewer();
        this._initEvents();
    }

    _requestServerProcess () {
        this._sessionId = create_UUID();

        // Create PsServer caller
        let psServerURL = window.location.protocol + "//" + window.location.hostname + ":" + this._port;
        if (this._reverseProxy) {
            psServerURL = window.location.protocol + "//" + window.location.hostname + "/httpproxy/" + this._port;
        }

        this._serverCaller = new ServerCaller(psServerURL, this._sessionId);

    }

    _createViewer() {
        let endpoint = "ws://" + window.location.hostname + ":11182";
        if (this._reverseProxy) {
            if ("http:" == window.location.protocol) {
                endpoint = "ws://" + window.location.hostname + "/wsproxy/11182";
            }
            else {
                endpoint = "wss://" + window.location.hostname + "/wsproxy/11182";
            }
        }
        this._viewer = new Communicator.WebViewer({
            containerId: "container",
            model: "_empty",
            endpointUri: endpoint,
            rendererType: Communicator.RendererType.Client
        });

        this._viewer.setCallbacks({
            sceneReady: () => {
                this._viewer.view.setBackgroundColor(new Communicator.Color(255, 255, 255), new Communicator.Color(230, 204, 179));
            },
            timeout: () => {
                this._serverCaller.CallServerPost("Clear");
                console.log("Timeout");
            }
        });

        callbacks = new Callbacks(this._viewer);

        this._viewer.start();
    }

    _initEvents() {
        let resizeTimer;
        let interval = Math.floor(1000 / 60 * 10);
        $(window).resize(() => {
        if (resizeTimer !== false) {
            clearTimeout(resizeTimer);
        }
        resizeTimer = setTimeout(() => {
            this._viewer.resizeCanvas();
        }, interval);
        });

        // Before page reload or close
        $(window).on('beforeunload', (e) => {
            this._serverCaller.CallServerPost("Clear");
        });

        // File uploading
        $("#UploadDlg").dialog({
            autoOpen: false,
            height: 380,
            width: 450,
            modal: true,
            title: "CAD file upload",
            closeOnEscape: true,
            position: {my: "center", at: "center", of: window},
            buttons: {
                'Cancel': () => {
                    $("#UploadDlg").dialog('close');
                }
            }
        });

        $("#UploadDlg").submit((e) => {
            // Cancel default behavior (abort form action)
            e.preventDefault();

            if (0 == e.target[0].files.length) {
                alert("Please select a file.");
                return;
            }

            // Get file name
            // let scModelName = e.target[0].files[0].name;
            // scModelName = scModelName.split(".").shift();
            const scModelName = "model";

            // Get options
            const entityIds = Number($("#checkEntityIds").prop('checked'));
            const sewModel = Number($("#checkSewModel").prop('checked'));
            const sewingTol = Number($("#sewingTol").val());

            const params = {
                entityIds: entityIds,
                sewModel: sewModel,
                sewingTol: sewingTol
            }

            const formData = new FormData(e.target);

            this._loadModel(params, formData, scModelName);

            $('#UploadDlg').dialog('close');
        });

        // Tolerance select
        $('#sewingTol').val(0.01);
        $('#classifyTol').val(0.0001);

        // Simple command
        $(".toolbarBtn").on("click", (e) => {
            let command = $(e.currentTarget).data("command");
            let isOn = $(e.currentTarget).data("on");

            switch(command) {
                case "Upload": {
                    $('#UploadDlg').dialog('open');
                } break;
                case "ClassifyFaces": {
                    const simplify = Number($("#checkSimplify").prop('checked'));
                    const tol = Number($('#classifyTol').val());
                    const params = { entityType: "FACE", simplify: simplify, tol: tol };
                    this._classifyEntities("ClassifyEntities", params);
                } break;
                case "ClassifyEdges": {
                    const simplify = Number($("#checkSimplify").prop('checked'));
                    const tol = Number($('#classifyTol').val());
                    const params = { entityType: "EDGE", simplify: simplify, tol: tol };
                    this._classifyEntities("ClassifyEntities", params);
                } break;
                case "FeatureRecognition": {
                    const simplify = Number($("#checkSimplify").prop('checked'));
                    const tol = Number($('#classifyTol').val());
                    const params = { simplify: simplify, tol: tol };
                    this._featureRecognition(command, params);
                } break;
                case "UnsetColor": {
                    this._unsetColor();
                } break;
                default: {} break;
            }
        });
    }

    _loadModel(params, formData, scModelName) {
        $("#loadingImage").show();
        this._serverCaller.CallServerPost("Clear").then(() => {
            this._requestServerProcess();
            this._serverCaller.CallServerPost("SetOptions", params).then(() => {
                this._serverCaller.CallServerSubmitFile(formData).then((arr) => {
                    let dataArr = Array.from(arr);
                    let unit = dataArr.shift();
                    unit = unit.toFixed(1);
                    unit = this._units[unit];
                    const area = dataArr.shift();
                    const volume = dataArr.shift();
                    const COG = [dataArr.shift(), dataArr.shift(), dataArr.shift()];
                    const BB = [dataArr.shift(), dataArr.shift(), dataArr.shift()];
                    const partCnt = dataArr.shift();
                    const bodyCnt = dataArr.shift();
                    const faceCnt = dataArr.shift();
                    const loopCnt = dataArr.shift();
                    const edgeCnt = dataArr.shift();

                    const partProps = '<ul><li>Unit: ' + unit + '</li>'
                    + '<li>Surface area: ' + area.toFixed(2) + '<br>'
                    + '<li>Volume: ' + volume.toFixed(2) + '</li>'
                    + '<li>Center of Gravity: X= ' + COG[0].toFixed(2) + ', Y= ' + COG[1].toFixed(2) + ', Z= ' + COG[2].toFixed(2) + '</li>'
                    + '<li>Bounding box: X= ' + BB[0].toFixed(2) + ', Y= ' + BB[1].toFixed(2) + ', Z= ' + BB[2].toFixed(2) + '</li>'
                    + '<li>Part count: ' + partCnt.toFixed(0) + '</li>'
                    + '<li>Body count: ' + bodyCnt.toFixed(0) + '</li>'
                    + '<li>Face count: ' + faceCnt.toFixed(0) + '</li>'
                    + '<li>Loop count: ' + loopCnt.toFixed(0) + '</li>'
                    + '<li>Edge count: ' + edgeCnt.toFixed(0) + '</li></ul>'

                    $("#dataInfo").html(partProps);

                    this._viewer.model.switchToModel(this._sessionId + "/" + scModelName).then((nodes) => {
                        this._bodyNodes = {};

                        // Get leaf nodes
                        const root = this._viewer.model.getAbsoluteRootNode();
                        let leafNodes = [];
                        this._traverseNodes(root, this._viewer.model, leafNodes);

                        let promiseArr = new Array(0);
                        for (let node of leafNodes) {
                            // Get node Body ID
                            promiseArr.push(this._viewer.model.getNodeProperties(node));
                            // Get face count of each body
                            promiseArr.push(this._viewer.model.getFaceCount(node));
                            // Get edge count of each body
                            promiseArr.push(this._viewer.model.getEdgeCount(node));
                        }

                        Promise.all(promiseArr).then((arr) => {
                            let count = 0;
                            for (let i = 0; i < arr.length; i += 3) {
                                const bodyProps = arr[i];
                                let bodyId = -1;
                                for (let key in bodyProps) {
                                    if ("myAttributes/Body ID" == key) {
                                        bodyId = Number(bodyProps[key]);
                                    }
                                }
                                if (-1 < bodyId) {
                                    this._bodyNodes[bodyId] = {
                                        nodeId: leafNodes[count],
                                        faceCnt: arr[i + 1],
                                        edgeCnt: arr[i + 2]
                                    }
                                }
                                count++;
                            }
                            this._setUnsetSelectionCallback(true, this._serverCaller, partProps, this._bodyNodes);
                            $("#loadingImage").hide();
                        });
                    });
                });
            });
        });
    }

    _traverseNodes(node, model, nodes) {
        let children = model.getNodeChildren(node);
        if (0 == children.length) {
            nodes.push(node);
            return;
        }
        for (let chile of children) {
            this._traverseNodes(chile, model, nodes);
        }
    }

    _classifyEntities(command, params) {
        if (0 == Object.keys(this._bodyNodes).length) return;

        $("#loadingImage").show();
        this._serverCaller.CallServerPost(command, params, "FLOAT").then((arr) => {
            if (0 < arr.length) {
                let colorIds = Array.from(arr);

                let typeCount;
                let typeColors = new Array(0);

                if ("FACE" == params.entityType) {
                    typeCount = [0, 0, 0, 0, 0, 0];
                    typeColors.push(new Communicator.Color(0, 0, 255));
                    typeColors.push(new Communicator.Color(0, 255, 255));
                    typeColors.push(new Communicator.Color(0, 255, 0));
                    typeColors.push(new Communicator.Color(255, 255, 0));
                    typeColors.push(new Communicator.Color(255, 128, 0));
                    typeColors.push(new Communicator.Color(255, 0, 0));
                }
                else if ("EDGE" == params.entityType) {
                    typeCount = [0, 0, 0, 0];
                    typeColors.push(new Communicator.Color(0, 0, 255));
                    typeColors.push(new Communicator.Color(0, 255, 0));
                    typeColors.push(new Communicator.Color(255, 255, 0));
                    typeColors.push(new Communicator.Color(255, 0, 0));
                }

                // Set colors by entity type
                let promiseArr = new Array(0);
                for (let i = 0; i < colorIds.length;) {
                    const bodyId = colorIds[i];
                    const entityCnt = colorIds[i + 1];
                    const colors = colorIds.slice(i + 2, i + 2 + entityCnt);
                    i += 2 + entityCnt;

                    const node = this._bodyNodes[bodyId].nodeId;

                    for (let j = 0; j < entityCnt; j++) {
                        let typeId = colors.shift();
                        typeCount[typeId]++;
                        if ("FACE" == params.entityType) promiseArr.push(this._viewer.model.setNodeFaceColor(node, j, typeColors[typeId]));
                        else if ("EDGE" == params.entityType) promiseArr.push(this._viewer.model.setNodeLineColor(node, j, typeColors[typeId]));
                    }
                }
                Promise.all(promiseArr);

                // Show count of each type
                let typeLabels;
                if ("FACE" == params.entityType) {
                    typeLabels = [
                        '<font color="#0000FF">Plane: ',
                        '<font color="#00FFFF">Cylinder: ',
                        '<font color="#00FF00">Cone: ',
                        '<font color="#FFFF00">Torus: ',
                        '<font color="#FF8000">Sphere: ',
                        '<font color="#FF0000">NURBS: '
                    ];
                }
                else if ("EDGE" == params.entityType) {
                    typeLabels = [
                        '<font color="#0000FF">Line: ',
                        '<font color="#00FF00">Circle: ',
                        '<font color="#FFFF00">Ellipse: ',
                        '<font color="#FF0000">NURBS: '
                    ];
                }
                let htmlStr = "";

                for (let i = 0; i < typeCount.length; i++) {
                    htmlStr += typeLabels[i] + typeCount[i] + '</font><br>'
                }

                $("#entityInfo").html(htmlStr);
                $("#loadingImage").hide();
            }
        });
    }

    _featureRecognition(command, params) {
        if (0 == Object.keys(this._bodyNodes).length) return;

        $("#loadingImage").show();
        this._serverCaller.CallServerPost(command, params, "FLOAT").then((arr) => {
            if (0 < arr.length) {
                let dataArr = Array.from(arr);
                let promiseArr = [];
                this._roHoles.length = 0;
                for (let i = 0; i < dataArr.length; ) {
                    const bodyId = dataArr[i];
                    const node = this._bodyNodes[bodyId].nodeId;

                    const dataCnt = dataArr[i + 1];

                    if (0 < dataCnt) {
                        const bodyHoles = dataArr.slice(i + 2, i + dataCnt + 2);

                        for (let j = 0; j < bodyHoles.length; ) {
                            const entityCnt = bodyHoles[j];
                            const dia = (bodyHoles[j + 1] * 2).toFixed(2);

                            let flg = false;
                            let roHole;

                            // Find same round hole
                            for (let k = 0; k < this._roHoles.length; k++) {
                                roHole = this._roHoles[k];
                                if (dia == roHole.dia) {
                                    flg = true;
                                    roHole.count++;
                                }
                            }

                            // Register new round hole
                            if (!flg) {
                                roHole = {
                                    shape: "RO",
                                    count: 1,
                                    nodeId: node,
                                    dia: dia,
                                    faceIds: []
                                }
                                this._roHoles.push(roHole);
                            }

                            for (let k = 0; k < entityCnt; k++) {
                                const faceId = bodyHoles[j + 2 + k];
                                roHole.faceIds.push(faceId);
                                // Change round hole faces' color
                                promiseArr.push(this._viewer.model.setNodeFaceColor(node, faceId, new Communicator.Color(0, 255, 255)));
                            }

                            j += entityCnt + 2;
                        }
                    }
                    i += dataCnt + 2;
                }
                Promise.all(promiseArr).then(() => {
                    // Show table of round hole
                    const table1 = $("<table><tbody>");
                    $("<tr class=table_title><td>Shape</td><td>Size</td><td>Count</td></tr>").appendTo(table1);
                    for (let i = 0; i < this._roHoles.length; i++) {
                        const roHole = this._roHoles[i];
                        const shape = roHole.shape;
                        const dia = this._roHoles[i].dia;
                        const count = roHole.count;
                        let faceIds = "";
                        $("<tr><td>" + shape + "</td><td class=table_text_r>" + dia + "</td><td class=table_text_r>" + count + "</td></tr>").appendTo(table1);
                    }
                    $("</tbody></table>").appendTo(table1);

                    $("#entityInfo").html("");
                    $("#entityInfo").append(table1);

                    $("#loadingImage").hide();

                    // Register tale click event
                    $('#entityInfo tr').on('click', (e) => {
                        this._viewer.model.resetModelHighlight().then(() => {
                            const row = e.currentTarget.sectionRowIndex;
                            if (0 < row) {
                                $('#entityInfo tr').each(function(i){
                                    if (0 < i) {
                                        if (row == i)
                                            $(this).css("background-color", "coral");
                                        else 
                                            $(this).css("background-color", "aliceblue");
                                    }
                                });

                                const roHole = this._roHoles[row - 1];
                                const nodeId = roHole.nodeId;
                                const faceIds = roHole.faceIds;

                                promiseArr.length = 0;
                                for (let faceId of faceIds) {
                                    promiseArr.push(this._viewer.model.setNodeFaceHighlighted(nodeId, faceId, true));
                                }
                            }
                            Promise.all(promiseArr);
                        });
                    });
                });
            }
        });
    }

    _unsetColor() {
        let promiseArr = new Array(0);
        for (let key in this._bodyNodes) {
            const node = this._bodyNodes[key].nodeId;
            const faceCnt = this._bodyNodes[key].faceCnt;
            for (let j = 0; j < faceCnt; j++) {
                promiseArr.push(this._viewer.model.unsetNodeFaceColor(node, j));
            }

            const edgeCnt = this._bodyNodes[key].edgeCnt;
            for (let j = 0; j < edgeCnt; j++) {
                promiseArr.push(this._viewer.model.unsetNodeLineColor(node, j));
            }
        }
        Promise.all(promiseArr);
        $("#entityInfo").html("");
    }

    _setUnsetSelectionCallback(isSet, serverCaller, partProps, bodyNodes) {
        callbacks.setParams(serverCaller, partProps, bodyNodes);

        function selectionArrayFunc(selEvents, removed) {
            callbacks.selectionArray(selEvents, removed);
        }

        if (isSet) {
            this._viewer.setCallbacks({
                selectionArray: selectionArrayFunc
            })
        }
        else {
            this._viewer.unsetCallbacks({
                selectionArray: selectionArrayFunc
            })
        }
    }
}

class Callbacks {
    constructor(viewer) {
        this._viewer = viewer;
        this._serverCaller;
        this._bodyNodes;
        this._partProperties = "";
        this._isBusy = false;
        this._entityType;
    }

    setParams (serverCaller, partProps, bodyNodes) {
        this._serverCaller = serverCaller;
        this._partProperties = partProps;
        this._bodyNodes = bodyNodes;
    }

    selectionArray(selEvents, removed) {
        if (0 == selEvents.length) {
            if (!this._isBusy) $("#dataInfo").html(this._partProperties);
            return;
        }

        const selEvent = selEvents.pop();
        const selItem = selEvent.getSelection();
        let promiseArr = [];

        const nodeId = selItem.getNodeId();
        let infoString = "Node ID: " + nodeId + ", ";

        promiseArr.push(this._viewer.model.getNodeProperties(nodeId));

        let entityId = -1;
        if (selItem.isFaceSelection()) {
            const face = selItem.getFaceEntity();
            entityId = face.getCadFaceIndex();
            infoString += "Face ID: " + entityId + "<br>";
            promiseArr.push(this._viewer.model.getFaceAttributes(nodeId, entityId));
            this._entityType = "FACE";
        }
        else if (selItem.isLineSelection()) {
            const edge = selItem.getLineEntity();
            entityId = edge.getLineId();
            infoString += "Edge ID: " + entityId + "<br>";
            promiseArr.push(this._viewer.model.getEdgeAttributes(nodeId, entityId));
            this._entityType = "EDGE";
        }
        $("#dataInfo").html(infoString);

        // Get Body ID
        let bodyId = -1;
        for(let key in this._bodyNodes) {
            if (nodeId == this._bodyNodes[key].nodeId) {
                bodyId = key;
                break
            }
        }

        if (-1 < bodyId && -1 < entityId) {
            const simplify = Number($("#checkSimplify").prop('checked'));
            const tol = Number($('#classifyTol').val());
            const params = { entityType: this._entityType, simplify: simplify, tol: tol, bodyId: bodyId, entityId: entityId };
            promiseArr.push(this._serverCaller.CallServerPost("InquiryEntityInfo", params, "FLOAT"));
        }

        this._isBusy = true;

        Promise.all(promiseArr).then((arr)=> {
            if (0 == arr.length) return;
            const bodyProps = arr[0];
            
            let infoString = $("#dataInfo").html();
            infoString += "Node properties:<ul>";
            for (let key in bodyProps) {
                infoString += "<li>" + key + " = " + bodyProps[key] + "</li>";
            }

            if (1 < arr.length) {
                const entityProps = arr[1];
                infoString += "</ul>Entity properties:<ul>";
                for (let attr of entityProps.attributes) {
                    infoString += "<li>" + attr["_title"] + " = " + attr["_value"] + "</li>";
                }
                infoString += "</ul>";
            }

            if (2 < arr.length) {
                const entityInfo = arr[2];
                const type = entityInfo[0];
                if ("FACE" == this._entityType) {
                    infoString += "<p>Face type: ";
                    switch (type) {
                        case 0: infoString += "Plane"; break;
                        case 1: 
                        {
                            let rad = entityInfo[1];
                            infoString += "Cylinder, Radius: " + rad.toFixed(2);
                        } break;
                        case 2: infoString += "Cone"; break;
                        case 3: infoString += "Torus"; break;
                        case 4:
                        {
                            let rad = entityInfo[1];
                            infoString += "Sphere, Radius: " + rad.toFixed(2);
                        } break;
                        case 5: infoString += "NURBS"; break;
                        default: break;
                    }
                }
                else if ("EDGE" == this._entityType) {
                    infoString += "<p>Edge type: ";
                    switch (type) {
                        case 0: infoString += "Line"; break;
                        case 1: 
                        {
                            let rad = entityInfo[1];
                            infoString += "Circle, Radius: " + rad.toFixed(2);
                        } break;
                        case 2: infoString += "Ellipse"; break;
                        case 3: infoString += "NURBS"; break;
                        default: break;
                    }
                }
                infoString += "</p>";

            }
            
            $("#dataInfo").html(infoString);
            this._isBusy = false;
        });
    }
}