package com.tree.service;

import org.springframework.stereotype.Service;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

@Service
public class TreeService {

    // C语言可执行文件路径
    private static final String CTREE_EXEC = "/Users/liu/Documents/trae_projects/1/backend/c-tree/tree";
    private static final String TREE_FILE = "tree_data.tmp";

    // 执行C语言命令并获取输出
    private String executeCCommand(String... args) throws IOException, InterruptedException {
        ProcessBuilder pb = new ProcessBuilder();
        pb.command(args);
        pb.redirectErrorStream(true);
        Process process = pb.start();
        
        StringBuilder output = new StringBuilder();
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()))) {
            String line;
            while ((line = reader.readLine()) != null) {
                output.append(line).append("\n");
            }
        }
        
        int exitCode = process.waitFor();
        if (exitCode != 0) {
            throw new RuntimeException("C command failed with exit code " + exitCode + ": " + output.toString());
        }
        
        return output.toString().trim();
    }

    // 从预序序列构建树
    public String buildFromPreorder(String preorder) throws IOException, InterruptedException {
        if (preorder == null || preorder.isEmpty()) {
            throw new IllegalArgumentException("预序序列不能为空");
        }
        
        // 调用C程序构建树
        executeCCommand(CTREE_EXEC, "build", preorder);
        return TREE_FILE;
    }

    // 计算树高
    public int treeHeight(String treeFile) throws IOException, InterruptedException {
        String output = executeCCommand(CTREE_EXEC, "height", treeFile);
        return Integer.parseInt(output);
    }

    // 后序遍历
    public String postorder(String treeFile) throws IOException, InterruptedException {
        return executeCCommand(CTREE_EXEC, "postorder", treeFile);
    }

    // 根到目标节点的路径
    public List<Character> findPath(String treeFile, char target) throws IOException, InterruptedException {
        String output = executeCCommand(CTREE_EXEC, "path", treeFile, String.valueOf(target));
        
        List<Character> path = new ArrayList<>();
        if (!output.equals("-1")) {
            // 解析路径字符串，如 "A->B->D" 转为 ['A', 'B', 'D']
            String[] nodes = output.split("->");
            for (String node : nodes) {
                if (!node.isEmpty()) {
                    path.add(node.charAt(0));
                }
            }
        }
        return path;
    }

    // 节点所处高度（BFS）
    public int getNodeLevel(String treeFile, char target) throws IOException, InterruptedException {
        String output = executeCCommand(CTREE_EXEC, "nodelevel", treeFile, String.valueOf(target));
        return Integer.parseInt(output);
    }

    // 释放树资源
    public void freeTree(String treeFile) {
        File file = new File(treeFile);
        if (file.exists()) {
            file.delete();
        }
    }
}