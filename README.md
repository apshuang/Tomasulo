# Tomasulo(Without ROB)
Sun Yat-Sen University 2024秋季学期 并行计算机体系结构大作业

### 项目特点
* 封装良好，易于修改
* 可视化程度好，易于展示
* 注释丰富，易于理解

### 基本假设
* 我们不对register和内存中的初始值做任何假设，所以所有对他们的取值都将获得一个string来指示其“表面值”，比如Reg[F1]，或者Mem[x1+32]
* 为了简化项目，我们不另外创建功能单元类（即memory unit、adder、multiplier等），而是直接将他们整合到RS或LoadBuffer部件中
* 写CDB实际上就是将result值传送给CommonDataBus这个部件，然后再由这个CDB将值传送出去，比如Add2完成了计算得到一个result，那么写CDB就是将所有依赖Add2的部件，全部改写为这个result。
* 功能单元采用“类FIFO”式的服务，即在所有已经ready的instruction中，保证先到的一定先服务
* 有一个关键的设计：比如某条指令，在第3周期完成，在第4周期写回，那么第4周期的时候，依赖这个result的所有指令才能拿到这个数，然后就可以进行对应的操作了。所以在这个模拟系统中，我们会在第3周期的时候，将数据送进CDB，然后CDB在第4周期的最初，将所有数据传送给其它组件，从而实现符合实际情况的设计。
* 我们假设branch指令的执行在操作数ready的情况下，就可以马上得到结果。
* 我们假设load指令需要2周期执行、add/sub需要2周期、mult需要6周期、div需要12周期；同时，我们设定adder和multiplier都只有一个，而自动计算好地址（即integer部件是无限的）（这些可以在`BasicDefine.h`中调整）。
* 以一个`fld f6,32(x2)`指令为例，它在第1周期完成issue，在第2周期开始执行，在第4周期执行完成，在第5周期写回。

### 基本架构
* 本项目中包含register（整型和浮点型）、LoadBuffer、StoreBuffer、ReservationStation（包括adder和multiplier）、InstructionDecoder、CommonDataBus以及一个Tomasulo类（用来将上述所有部件整合在一起）。
* 其中InstructionDecoder负责将指令读取并解析，将指令发送到对应的保留站（包括load/store buffer），同时，它还负责branch指令的执行，只要两个操作数准备好了，他就可以进行比较，并决定是否跳转（在branch指令的操作数ready之前，必须阻塞在这里，不能issue后面的指令）
* 其中load/store buffer和保留站，就是用来缓存指令的地方，它们在计算单元有空闲的时候，自动将指令发送到计算单元上进行计算（本项目没有抽象出这样一个类，而是直接通过模拟remainingTime来实现）。而在计算完毕之后，也由它们来将result发送到CDB上。
* register和CDB会根据指令来设置自己的字面值，比如一条`fld f6,32(x2)`指令，此时就会让`f6`寄存器变成`Load1`，从而后续引用`f6`寄存器的值时，就会自动引用`Load1`。


### 需要注意的点
* 因为本项目都是用字面值，所以值太长的话（比如Mem[44 + Regs[x3]] - Mem[32 + Regs[x2]]这种），有可能会导致output的时候出错（因为使用了格式化输出来控制），如果出现string too long的报错，那就说明是某些输出太长了。
* 目前还只是通过预设值来结束循环，比如`bne x1,x3,Loop`，那么在一开始就需要预设`x3`为`Regs[x1] + 8 + 8 + 8`，这样才能保证循环能够正常结束。该项目目前还没有通过输入来进行值预设，所以在Registers中有一段`#if ISSUENUM == 2`，在该情况下，会通过代码预设

### 有待优化的点
* 增加一个预设环节，能够通过输入来预设某些寄存器以及某些内存位置的初始值。