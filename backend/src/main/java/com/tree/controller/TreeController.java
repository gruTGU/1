package com.tree.controller;

import com.tree.dto.ApiResponse;
import com.tree.dto.BuildTreeRequest;
import com.tree.service.TreeService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@RestController
@RequestMapping("/api/tree")
public class TreeController {

    @Autowired
    private TreeService treeService;

    // 构建树
    @PostMapping("/build")
    public ApiResponse<String> buildTree(@RequestBody BuildTreeRequest request) {
        try {
            String preorder = request.getPreorder();
            if (preorder == null || preorder.isEmpty()) {
                return ApiResponse.error("预序序列不能为空");
            }
            String treeFile = treeService.buildFromPreorder(preorder);
            return ApiResponse.success(treeFile, "构建树成功");
        } catch (Exception e) {
            return ApiResponse.error("构建树失败：" + e.getMessage());
        }
    }

    // 获取树高
    @GetMapping("/height")
    public ApiResponse<Integer> getTreeHeight() {
        try {
            int height = treeService.treeHeight("tree_data.tmp");
            return ApiResponse.success(height, "获取树高成功");
        } catch (Exception e) {
            return ApiResponse.error("获取树高失败：" + e.getMessage());
        }
    }

    // 获取后序遍历
    @GetMapping("/postorder")
    public ApiResponse<String> getPostorder() {
        try {
            String postorder = treeService.postorder("tree_data.tmp");
            return ApiResponse.success(postorder, "获取后序遍历成功");
        } catch (Exception e) {
            return ApiResponse.error("获取后序遍历失败：" + e.getMessage());
        }
    }

    // 获取根到节点路径
    @GetMapping("/path/{target}")
    public ApiResponse<List<Character>> getPath(@PathVariable char target) {
        try {
            List<Character> path = treeService.findPath("tree_data.tmp", target);
            if (path.isEmpty()) {
                return ApiResponse.error("未找到目标节点");
            }
            return ApiResponse.success(path, "获取路径成功");
        } catch (Exception e) {
            return ApiResponse.error("获取路径失败：" + e.getMessage());
        }
    }

    // 获取节点高度
    @GetMapping("/node-level/{target}")
    public ApiResponse<Integer> getNodeLevel(@PathVariable char target) {
        try {
            int level = treeService.getNodeLevel("tree_data.tmp", target);
            if (level == -1) {
                return ApiResponse.error("未找到目标节点");
            }
            return ApiResponse.success(level, "获取节点高度成功");
        } catch (Exception e) {
            return ApiResponse.error("获取节点高度失败：" + e.getMessage());
        }
    }

    // 删除树
    @DeleteMapping
    public ApiResponse<Boolean> deleteTree() {
        try {
            treeService.freeTree("tree_data.tmp");
            return ApiResponse.success(true, "删除树成功");
        } catch (Exception e) {
            return ApiResponse.error("删除树失败：" + e.getMessage());
        }
    }
}