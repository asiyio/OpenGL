## 🚀 阶段 1：基础框架 + PBR 材质渲染（无环境光照）

### ✅ 任务目标：

- 初始化 OpenGL + GLFW + GLAD + ImGui 项目框架；
- 加载一个 PBR 材质球模型（可用球体 + 金属度/粗糙度贴图）；
- 实现 Cook-Torrance PBR 着色器（无 IBL）；
- ImGui 可调节材质参数（metallic, roughness）；
- 支持摄像机视角控制（鼠标+WASD）。

### 🧱 技术点：

- Shader 结构设计（统一材质传入）
- 基本光照结构体封装
- Gamma 校正、线性空间处理

------

## 🌅 阶段 2：加入 IBL 环境光照 + HDR 天空盒切换

### ✅ 任务目标：

- 加载 HDR 环境贴图，生成：
    - Irradiance Map（漫反射环境光）
    - Prefilter Map（镜面反射）
    - BRDF LUT（预积分查找表）
- 实现完整 IBL 管线（基于环境贴图的光照）；
- 支持多组 HDR skybox 切换；
- ImGui 实时切换环境贴图；
- Skybox cube 渲染（基于视角固定 camera）。

------

## 💡 阶段 3：多模型支持 + 动态光照调节

### ✅ 任务目标：

- 支持加载多个模型（球体、扫描模型、武器等）；
- ImGui 面板：切换不同模型；
- 动态光源支持（位置、强度、颜色）；
- ImGui 实时调节光源属性；
- 支持多个光源 + Shader 动态管理；

------

## 🎬 阶段 4：后期处理（Bloom + 色调映射）

### ✅ 任务目标：

- 实现帧缓冲渲染（HDR FBO）；
- 实现基于亮度提取的 Bloom 效果（高斯模糊）；
- 加入色调映射（Reinhard、ACES 等）；
- ImGui 控制 Bloom 强度、色调映射模式；
- 支持 Gamma 校正与线性色空间切换。