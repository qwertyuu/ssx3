// Ghidra headless script: dump decompiled pseudocode for all functions
// @category SSX3
// @author SSX3 Decomp

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.FunctionManager;
import ghidra.util.task.ConsoleTaskMonitor;

import java.io.File;
import java.io.FileWriter;

public class DecompileAll extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        String outputDir = args.length > 0 ? args[0] : "/samples/ghidra_out";

        File outDir = new File(outputDir);
        if (!outDir.exists()) {
            outDir.mkdirs();
        }

        DecompInterface decomp = new DecompInterface();
        decomp.openProgram(currentProgram);

        FunctionManager fm = currentProgram.getFunctionManager();
        FunctionIterator funcs = fm.getFunctions(true);

        int count = 0;
        while (funcs.hasNext()) {
            Function func = funcs.next();
            String name = func.getName();
            String addr = func.getEntryPoint().toString();

            DecompileResults results = decomp.decompileFunction(func, 30, monitor);
            if (results.decompileCompleted() && results.getDecompiledFunction() != null) {
                String code = results.getDecompiledFunction().getC();
                String filename = addr + "__" + name.replaceAll("[^a-zA-Z0-9_]", "_") + ".c";
                File outFile = new File(outDir, filename);
                FileWriter writer = new FileWriter(outFile);
                writer.write("// Function: " + name + " @ " + addr + "\n");
                writer.write("// Size: " + func.getBody().getNumAddresses() + " bytes\n\n");
                writer.write(code);
                writer.close();
                count++;
            }
        }

        decomp.dispose();
        println("Decompiled " + count + " functions to " + outputDir);
    }
}
