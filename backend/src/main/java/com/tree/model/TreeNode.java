package com.tree.model;

public class TreeNode {
    private char data;
    private TreeNode lchild;
    private TreeNode rchild;

    public TreeNode(char data) {
        this.data = data;
        this.lchild = null;
        this.rchild = null;
    }

    public char getData() {
        return data;
    }

    public void setData(char data) {
        this.data = data;
    }

    public TreeNode getLchild() {
        return lchild;
    }

    public void setLchild(TreeNode lchild) {
        this.lchild = lchild;
    }

    public TreeNode getRchild() {
        return rchild;
    }

    public void setRchild(TreeNode rchild) {
        this.rchild = rchild;
    }
}