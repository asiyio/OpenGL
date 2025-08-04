## Cook-Torrance 模型中的 NDF 选择

在 Cook-Torrance BSDF 中，**NDF（Normal Distribution Function，法线分布函数）**用于描述微表面法线的统计分布，是整个镜面反射模型的核心组成部分之一。不同的 NDF 决定了高光的形状和能量分布，也极大影响材质的真实感。

------

### ✅ 1. **Beckmann 分布**

#### 数学形式：

$$
D(\mathbf{N}, \mathbf{H}) = \frac{1}{\pi \alpha^2 \cos^4\theta} \exp\left(-\frac{\tan^2\theta}{\alpha^2}\right)
$$

其中：

- $\alpha$：粗糙度参数
- $\theta_h$：微表面法线与宏观法线之间的夹角$(\theta_h = \arccos(n \cdot h))$

#### 特点：

- 起源于统计热力学，具有良好的物理基础；
- 高光边缘衰减快，模拟陶瓷、玻璃等光滑材质较真实；
- 不如 GGX 适合模拟粗糙材质（高光范围小）；

------

### ✅ 2. **GGX（Trowbridge-Reitz）分布** —— 实时渲染首选

#### 数学形式（常用形式）：

$$
D(h) = \frac{\alpha^2}{\pi \left[(n \cdot h)^2(\alpha^2 - 1) + 1\right]^2}
$$

或在某些实现中写为：
$$
D(h) = \frac{\alpha^2}{\pi \left[(n \cdot h)^2(\alpha^2 - 1) + 1\right]^2} \quad \text{其中 } \alpha = roughness^2
$$

#### 特点：

- 较 Beckmann 有“更长的尾部”：高光在边缘不容易快速衰减，适合模拟粗糙金属或其他散射强的表面；
- 保留更多 grazing-angle 反射（低角度反射）；
- 广泛应用于实时渲染（Unity, Unreal, Filament 等）；
- 更稳定的能量守恒，结合 Smith G 几何项时表现更自然；

------

### ✅ 3. **Phong 分布（不推荐）**

#### 数学形式：

$$
D(h) = \frac{(n + 2)}{2\pi} \cdot (n \cdot h)^n
$$



#### 特点：

- 过时模型，来源于经典 Phong 模型；
- 不满足能量守恒；
- 不具备真实物理意义，不再用于现代 PBR 系统；

------

### 🧠 总结：选择建议

| 场景                         | 建议 NDF        | 原因                           |
| ---------------------------- | --------------- | ------------------------------ |
| 实时渲染（游戏、实时预览）   | GGX             | 真实、性能好、模拟粗糙表面出色 |
| 光滑电介质材质（玻璃、陶瓷） | Beckmann        | 高光集中、符合实际测量值       |
| 离线渲染（如 Cycles）        | Beckmann 或 GGX | 根据材质选择，更注重真实性     |
| 粗糙金属、塑料等粗糙材质     | GGX             | 模拟能量扩散更自然             |

------

### ✅ **GGX + Smith G + Schlick Fresnel**

这是目前实时图形中最广泛使用的组合，能提供高质量、高性能、真实感强的镜面反射模型。