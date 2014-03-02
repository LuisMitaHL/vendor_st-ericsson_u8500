package com.stericsson.sdk.signing.a2;

import java.io.File;

/**
 * 
 * @author xolabju
 * 
 */
public final class ListTestClasses {

    private static final String ROOT = "C:\\git_repo\\flash_tool_backend\\com.stericsson.sdk.signing.test\\src\\";

    static int i = 0;

    private ListTestClasses() {
    }

    /**
     * 
     * @param args
     *            tbd
     */
    public static void main(String[] args) {
        File root = new File(ROOT);
        listClasses(root);
        System.out.println(i + " classes");
    }

    private static void listClasses(File root) {
        File[] files = root.listFiles();
        for (File file : files) {
            if (file.isFile()) {
                String name =
                    "\"" + file.getAbsolutePath().replace(ROOT, "").replace("\\", ".").replace(".java", "") + "\",";
                if (name.endsWith("Test\",")) {
                    System.out.println(name);
                    i++;
                }
            } else if (file.isDirectory()) {
                listClasses(file);
            }
        }

    }
}
