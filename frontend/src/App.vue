<template>
  <div class="app-container">
    <h1 class="title">树结构可视化系统</h1>

    <div class="main-content">
      <!-- 构建树 -->
      <el-card class="card">
        <template #header>
          <div class="card-header">
            <span>构建树</span>
          </div>
        </template>
        <div class="card-body">
          <el-input
            v-model="preorderInput"
            placeholder="请输入前序序列（#表示空），例如：ABD##E##CF###"
            maxlength="2047"
            show-word-limit
            class="input-preorder"
          />
          <el-button type="primary" @click="buildTree" class="btn-build">构建树</el-button>
        </div>
      </el-card>

      <!-- 树可视化 -->
      <el-card class="card tree-card" v-if="treeBuilt">
        <template #header>
          <div class="card-header">
            <span>树状图展示</span>
          </div>
        </template>
        <div class="card-body">
          <div ref="graphRef" class="tree-graph-container"></div>
        </div>
      </el-card>

      <!-- 树信息 -->
      <el-card class="card info-card" v-if="treeBuilt">
        <template #header>
          <div class="card-header">
            <span>树信息</span>
          </div>
        </template>
        <div class="card-body">
          <div class="info-item">
            <span class="label">树高：</span>
            <span class="value">{{ treeHeight }}</span>
          </div>
          <div class="info-item">
            <span class="label">后序遍历：</span>
            <span class="value">{{ postorderResult }}</span>
          </div>
        </div>
      </el-card>

      <!-- 路径查询 -->
      <el-card class="card compact-card" v-if="treeBuilt">
        <template #header>
          <div class="card-header">
            <span>路径查询</span>
          </div>
        </template>
        <div class="card-body">
          <div class="query-section">
            <el-input
              v-model="pathTarget"
              placeholder="输入目标节点字符"
              maxlength="1"
              show-word-limit
              class="input-target"
            />
            <el-button type="primary" @click="queryPath" class="btn-query">查询路径</el-button>
          </div>
          <div v-if="pathResult" class="result-section">
            <span class="label">路径：</span>
            <span class="value path-value">{{ pathResult }}</span>
          </div>
        </div>
      </el-card>

      <!-- 节点高度查询 -->
      <el-card class="card compact-card" v-if="treeBuilt">
        <template #header>
          <div class="card-header">
            <span>节点高度查询</span>
          </div>
        </template>
        <div class="card-body">
          <div class="query-section">
            <el-input
              v-model="levelTarget"
              placeholder="输入目标节点字符"
              maxlength="1"
              show-word-limit
              class="input-target"
            />
            <el-button type="primary" @click="queryNodeLevel" class="btn-query">查询高度</el-button>
          </div>
          <div v-if="levelResult" class="result-section">
            <span class="label">节点高度：</span>
            <span class="value">{{ levelResult }}</span>
          </div>
        </div>
      </el-card>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, nextTick } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'
import { Graph } from '@antv/g6'

const preorderInput = ref('')
const treeBuilt = ref(false)
const treeHeight = ref(null)
const postorderResult = ref('')
const pathTarget = ref('')
const pathResult = ref('')
const levelTarget = ref('')
const levelResult = ref('')
const treeData = ref(null)
const graphRef = ref(null)
const highlightPath = ref([])
const highlightNode = ref(null)

// 构建树
const buildTree = async () => {
  if (!preorderInput.value) {
    ElMessage.warning('请输入前序序列')
    return
  }

  try {
    const response = await axios.post('/api/tree/build', {
      preorder: preorderInput.value
    })
    
    if (response.data.success) {
      treeBuilt.value = true
      ElMessage.success('构建树成功')
      // 获取树信息
      await getTreeInfo()
      // 解析树结构
      parseTreeStructure(preorderInput.value)
    } else {
      ElMessage.error(response.data.message)
    }
  } catch (error) {
    ElMessage.error('构建树失败：' + error.message)
  }
}

// 获取树信息
const getTreeInfo = async () => {
  try {
    // 获取树高
    const heightResponse = await axios.get('/api/tree/height')
    if (heightResponse.data.success) {
      treeHeight.value = heightResponse.data.data
      if (treeHeight.value < 5) {
        ElMessage.warning('提示：你输入的树高度小于 5，不满足题目“高度不低于5”的要求，请换一组前序测试。')
      }
    }

    // 获取后序遍历
    const postorderResponse = await axios.get('/api/tree/postorder')
    if (postorderResponse.data.success) {
      postorderResult.value = postorderResponse.data.data
    }
  } catch (error) {
    ElMessage.error('获取树信息失败：' + error.message)
  }
}

// 查询路径
const queryPath = async () => {
  if (!pathTarget.value) {
    ElMessage.warning('请输入目标节点')
    return
  }

  try {
    const response = await axios.get(`/api/tree/path/${pathTarget.value}`)
    if (response.data.success) {
      const path = response.data.data
      pathResult.value = path.join('->')
      highlightPath.value = path
      highlightNode.value = null
      // 重新渲染树
      renderTree()
    } else {
      pathResult.value = '未找到节点 '
      ElMessage.warning(response.data.message)
      highlightPath.value = []
      renderTree()
    }
  } catch (error) {
    pathResult.value = ''
    ElMessage.error('查询路径失败：' + error.message)
    highlightPath.value = []
    renderTree()
  }
}

// 查询节点高度
const queryNodeLevel = async () => {
  if (!levelTarget.value) {
    ElMessage.warning('请输入目标节点')
    return
  }

  try {
    const response = await axios.get(`/api/tree/node-level/${levelTarget.value}`)
    if (response.data.success) {
      levelResult.value = response.data.data
      highlightNode.value = levelTarget.value
      highlightPath.value = []
      // 重新渲染树
      renderTree()
    } else {
      levelResult.value = '未找到节点 '
      ElMessage.warning(response.data.message)
      highlightNode.value = null
      renderTree()
    }
  } catch (error) {
    levelResult.value = ''
    ElMessage.error('查询节点高度失败：' + error.message)
    highlightNode.value = null
    renderTree()
  }
}

// 解析树结构
const parseTreeStructure = (preorder) => {
  let index = 0
  
  const buildTreeRecursive = () => {
    if (index >= preorder.length || preorder[index] === '#') {
      index++
      return null
    }
    
    const node = {
      id: preorder[index],
      label: preorder[index],
      children: []
    }
    index++
    
    const leftChild = buildTreeRecursive()
    const rightChild = buildTreeRecursive()
    
    if (leftChild) node.children.push(leftChild)
    if (rightChild) node.children.push(rightChild)
    
    return node
  }
  
  treeData.value = buildTreeRecursive()
  
  // 使用 nextTick 确保 DOM 已更新后再渲染
  nextTick(() => {
    renderTree()
  })
}

// 渲染树
const renderTree = () => {
  console.log('=== renderTree 开始执行 ===')
  console.log('treeData.value:', treeData.value)
  console.log('graphRef.value:', graphRef.value)
  
  if (!treeData.value) {
    console.error('treeData 为空，无法渲染')
    return
  }
  
  if (!graphRef.value) {
    console.error('graphRef 为空，DOM 元素未找到')
    return
  }
  
  console.log('开始清空容器...')
  graphRef.value.innerHTML = ''
  
  const width = graphRef.value.offsetWidth || 600
  const height = graphRef.value.offsetHeight || 500
  console.log(`画布尺寸: ${width}x${height}`)
  
  // 转换树数据格式
  console.log('开始转换树数据格式...')
  const graphData = treeToGraphData(treeData.value)
  console.log('graphData:', JSON.stringify(graphData, null, 2))
  
  if (!graphData) {
    console.error('graphData 转换失败')
    return
  }
  
  // 应用样式
  console.log('开始应用样式...')
  applyStylesToGraphData(graphData)
  
  console.log('开始创建 Graph...')
  
  const graph = new Graph({
    container: graphRef.value,
    width,
    height,
    modes: {
      default: ['drag-canvas', 'zoom-canvas', 'drag-node']
    },
    defaultNode: {
      type: 'circle',
      size: 52,
      label: true,
      labelCfg: {
        position: 'center',
        offsetY: 0,
        style: {
          fill: '#000000',
          fontSize: 20,
          fontWeight: 'bold',
          textAlign: 'center',
          textBaseline: 'middle'
        }
      },
      style: {
        fill: '#E3F2FD',
        stroke: '#1976D2',
        lineWidth: 2,
        cursor: 'pointer'
      }
    },
    defaultEdge: {
      type: 'cubic-vertical',
      style: {
        stroke: '#90A4AE',
        lineWidth: 2,
        endArrow: true
      }
    },
    layout: {
      type: 'dendrogram',
      direction: 'TB',
      nodeSep: 48,
      rankSep: 80
    },
    data: graphData
  })
  
  console.log('Graph 创建成功:', graph)
  
  // G6 v5 需要手动调用 render() 方法
  graph.render()
  
  console.log('Graph 渲染完成')
  console.log('容器内容:', graphRef.value.innerHTML)
  
  // 自适应视图，调整缩放比例以适应容器
  graph.fitView(8)
  
  console.log('=== renderTree 执行完成 ===')
}

// 计算树深度
const calculateTreeDepth = (node) => {
  if (!node) return 0
  if (!node.children || node.children.length === 0) return 1
  return 1 + Math.max(...node.children.map(calculateTreeDepth))
}

// 转换为G6图数据格式（nodes和edges）
const treeToGraphData = (treeData) => {
  if (!treeData) return { nodes: [], edges: [] }
  
  const nodes = []
  const edges = []
  
  const convertNode = (node, parentId = null) => {
    const nodeId = String(node.id)
    
    // 添加节点
    nodes.push({
      id: nodeId,
      label: nodeId
    })
    
    // 添加边
    if (parentId) {
      edges.push({
        source: parentId,
        target: nodeId
      })
    }
    
    // 递归处理子节点
    if (node.children && node.children.length > 0) {
      node.children.forEach(child => {
        convertNode(child, nodeId)
      })
    }
  }
  
  convertNode(treeData)
  
  return { nodes, edges }
}

// 应用样式到 Graph 数据
const applyStylesToGraphData = (graphData) => {
  if (!graphData) return
  
  console.log('开始应用样式到 graphData:', graphData)
  
  // 处理节点
  if (graphData.nodes) {
    graphData.nodes.forEach(node => {
      const id = String(node.id)
      const baseStyle = {
        fill: '#E3F2FD',
        stroke: '#1976D2',
        lineWidth: 2,
        labelText: node.label,
        labelFill: '#1F1F1F',
        labelFontSize: 18,
        labelFontWeight: 600,
        labelTextAlign: 'center',
        labelTextBaseline: 'middle'
      }
      
      // 路径高亮
      if (highlightPath.value.includes(id)) {
        node.style = {
          ...baseStyle,
          fill: '#FFD54F',
          stroke: '#FF8F00',
          lineWidth: 3,
          labelFill: '#4E342E'
        }
      }
      // 节点高亮
      else if (highlightNode.value === id) {
        node.style = {
          ...baseStyle,
          fill: '#EF5350',
          stroke: '#C62828',
          lineWidth: 3,
          labelFill: '#FFFFFF'
        }
      } else {
        node.style = baseStyle
      }
    })
  }
  
  // 处理边（路径高亮）
  if (graphData.edges && highlightPath.value.length > 1) {
    graphData.edges.forEach(edge => {
      const source = String(edge.source)
      const target = String(edge.target)
      
      for (let i = 0; i < highlightPath.value.length - 1; i++) {
        if (highlightPath.value[i] === source && highlightPath.value[i + 1] === target) {
          edge.style = {
            stroke: '#FF8F00',
            lineWidth: 3
          }
          break
        }
      }
    })
  }
  
  console.log('样式应用完成:', graphData)
}

// 监听容器大小变化
const handleResize = () => {
  if (graphRef.value) {
    renderTree()
  }
}

onMounted(() => {
  window.addEventListener('resize', handleResize)
})
</script>

<style scoped>
.app-container {
  max-width: 1400px;
  margin: 0 auto;
  padding: 20px;
  background: linear-gradient(180deg, #f7f9fc 0%, #eef3f9 100%);
  min-height: 100vh;
}

.title {
  text-align: center;
  margin-bottom: 30px;
  color: #333;
  font-size: 28px;
  font-weight: 700;
}

/* 左侧：输入信息和树状图；右侧：其他功能 */
.main-content {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 24px;
}

/* 左侧布局：输入信息 + 树状图 */
.card:first-child {
  grid-column: 1;
  grid-row: 1;
}

.card.tree-card {
  grid-column: 1;
  grid-row: 2;
}

/* 右侧布局：树信息 + 路径查询 + 节点高度查询 */
.card:nth-child(3) {
  grid-column: 2;
  grid-row: 1;
}

.card:nth-child(4) {
  grid-column: 2;
  grid-row: 2;
}

.card:last-child {
  grid-column: 2;
  grid-row: 3;
}

/* 圆角美化的卡片 */
.card {
  margin-bottom: 24px;
  border-radius: 12px !important;
  overflow: hidden;
  background: #ffffff;
  box-shadow: 0 4px 18px 0 rgba(15, 30, 50, 0.08);
  transition: all 0.3s ease;
}

/* 调整Element Plus卡片的默认内边距，确保高度一致 */
.card .el-card__body {
  padding: 16px !important;
  margin: 0 !important;
}

.card:hover {
  box-shadow: 0 4px 16px 0 rgba(0, 0, 0, 0.12);
}

.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  border-radius: 12px 12px 0 0 !important;
}

.card :deep(.el-card__header) {
  background: #f4f7fb;
  border-bottom: 1px solid #e6edf5;
  color: #1f2d3d;
}

.card-body {
  padding: 16px 0;
}

.info-card .el-card__body,
.compact-card .el-card__body {
  min-height: 120px;
}

.compact-card .card-body {
  padding: 12px 0;
}

/* 圆角输入框 */
.input-preorder,
.input-target {
  border-radius: 8px !important;
  border: 1px solid #dcdfe6 !important;
  transition: all 0.3s ease;
}

.input-preorder:focus,
.input-target:focus {
  border-color: #409eff !important;
  box-shadow: 0 0 0 2px rgba(64, 158, 255, 0.2) !important;
}

.input-preorder {
  width: 70%;
  margin-right: 12px;
}

/* 圆角按钮 */
.btn-build,
.btn-query {
  border-radius: 8px !important;
  transition: all 0.3s ease;
}

.btn-build:hover,
.btn-query:hover {
  transform: translateY(-1px);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.15);
}

.btn-build {
  width: 28%;
}

.info-item {
  margin-bottom: 12px;
}

.label {
  display: inline-block;
  width: 90px;
  font-weight: bold;
  color: #666;
}

.value {
  color: #333;
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
}

.query-section {
  display: flex;
  margin-bottom: 12px;
}

.input-target {
  width: 60%;
  margin-right: 12px;
}

.btn-query {
  width: 38%;
}

.result-section {
  margin-top: 8px;
}

.path-value {
  font-family: monospace;
  color: #409eff;
  font-weight: 500;
}

/* 树状图容器样式 - 直接显示，取消框中框 */
.tree-graph-container {
  width: 100%;
  height: 640px;
  background: radial-gradient(circle at top, #ffffff 0%, #f7faff 100%);
  transition: all 0.3s ease;
}

/* 确保树状图占满整个容器 */
.tree-graph-container canvas {
  width: 100% !important;
  height: 100% !important;
  border-radius: 0;
}

/* 适配16:9屏幕的响应式设计 */
@media (min-width: 1200px) {
  .app-container {
    max-width: 1600px;
  }
  
  .main-content {
    gap: 28px;
  }
  
  .tree-graph-container {
    height: 720px;
  }
}

@media (max-width: 768px) {
  .main-content {
    grid-template-columns: 1fr;
    gap: 16px;
  }
  
  .card.tree-card {
    grid-column: 1 / 2;
  }
  
  .input-preorder,
  .btn-build,
  .input-target,
  .btn-query {
    width: 100%;
    margin-right: 0;
    margin-bottom: 12px;
  }
  
  .query-section {
    flex-direction: column;
  }
  
  .tree-graph-container {
    height: 450px;
  }
}
</style>
