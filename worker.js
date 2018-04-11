self.addEventListener('message', function(e) {
    const blob = e.data.blob;
    if (blob === undefined) {
        console.log("Unknown message from html");
        return;
    }
    check_repair(blob);
});

self.importScripts("filecheck.js");

let last_file_name = undefined;

function check_repair(blob) {

    var filename = blob.name;

    if (last_file_name !== undefined) {
        console.log("unlinking file");
        Module.FS_unlink(last_file_name);
    }
    last_file_name = filename;

    var fr = new FileReader();
    fr.readAsArrayBuffer(blob);

    fr.onload = function (){
        var data = new Uint8Array(fr.result); // base64 to Uint8 for emscripten

        Module['FS_createDataFile'](".", filename, data, true, true);

        Module.ccall("js_check_repair", // c function name
                undefined, // return
                ["string"], // param
                [filename]
        );

        const report_str = Module.FS_readFile("report.txt", {encoding:'utf8'});
        self.postMessage({"report": report_str});
    } // fr.onload
}
