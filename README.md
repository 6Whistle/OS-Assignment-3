# 운영체제 Assignment 3

### 이름 : 이준휘

### 학번 : 2018202046

### 교수 : 최상호 교수님

### 강의 시간 : 금 1, 2


## 1. Introduction

#### 해당 과제는 3 가지의 소과제로 나누어 진행된다. 첫 번째 과제에서는 자식 프로세스 또

```
는 Thread에서 파일을 읽어 더한 값을 부모에게 전달하는 작업을 수행한다. 해당 과정은
fork와 thread를 통해 각각 구현하며 결과값(value)이 MAX_PROCESSES가 8 일 때와 64 일
때 어떻게 달라지는지 확인한다. 두 번째 과제에서는 SCHED_RR, SCHED_FIFO,
SCHED_OTHER 스케줄러를 통해 fork를 사용하는 프로그램을 돌리고, 경과 시간을
priority나 nice 값에 따라 비교한다. 마지막 과제는 pid에 해당하는 task_struct 내용과
fork 수를 추적하는 모듈을 만든다. 이 때 fork 횟수를 추적하기 위해 task_struct와 fork와
관련된 system call을 변형시키며, task_struct의 구조를 파악하여 이를 출력한다.
```
## 2. Conclusion & Analysis

## A. Assignment 3 - 1

해당 파일은 numgen.c로 MAX_PROCESSES의 2 배만큼 순차적인 변수를 temp.txt에 생성한다.


해당 파일은 fork.c로 MAX_PROCESS만큼 프로세스를 생성하여 병렬 덧셈 연산을 수행한다.
우선 start와 end는 숫자의 구간을 의미하며 0 ~ MAX_PROCESSES * 2로 설정한다. 이후
clock_gettime()함수를 이용하여 timespec 구조체 startTime의 시작 시간을 기록한다. 다음으로
fork()를 통해 자식 프로세스를 만든 후 자식 프로세스에서는 다음 반복문을 수행한다.
반복문에서는 end – start > 1 이상, 즉 구간 안에 2 개의 수 이상이 있을 때만 반복을 수행한다.
if문을 통해 만약 구간 내에 2 개의 수가 있을 경우 temp.txt에서 해당 순서의 수 2 개를 읽어온
다. 그 후 해당 프로세스는 두 수를 더한 값을 exit의 인자로 사용하여 종료한다.
만약 구간 안에 수가 2 개 초과일 경우에는 div를 통해 구간을 나눌 곳을 지정한 후 2 번의 fork
를 통해 2 개의 자식을 생성한다. 이후 하나의 자식은 start ~ div까지, 다른 자식은 div ~ end까
지 구간을 다시 반복을 통해 수행하며 현 프로세스는 두 pid를 기다린다. 이 때 반환되는 값은
leftState와 rightState에 저장된다.


**Waitpid()로 받은 leftState과 rightState 변수의 경우 exit()의 인자(상위 8 bits) 뿐만 아니라
종료된 상태(하위 8 bits) 또한 포함하기 때문에 해당 변수를 그대로 사용하면 안 된다. 따라서
WEXITSTATUS() 또는 >> 8 을 통해 해당 값을 추출해야 한다.**

가장 부모 프로세서는 waitpid를 통해 결과를 반환 받는다. 이후 clock_gettime()을 통해 종료
시간을 측정하고 걸린 시간 t를 계산한다. 그 후 해당 결과들을 출력하고 프로그램을 종료한다.

위의 함수는 thread를 이용하여 병렬 덧셈 연산을 수행하는 thread.c 파일이다.
Void *pthread_ex(void *arg) 함수는 thread에서 수행할 함수다. 우선 void *arg에서 저장된 start
와 end를 추출한다. 그리고 함수 반환 시 전달할 heap 변수 ret를 미리 생성한다. 만약 현재
구간 안에 2 개의 수만 있을 경우 temp.txt에서 해당 위치의 값을 읽어 들인다. 그 후 ret에 읽
은 두 수를 더한 값을 저장하고 해당 ret를 반환한다.
만약 두 구간 안에 2 개의 수만 있지 않을 경우 구간을 분리하는 작업을 수행한다. 우선 point 1


과 point2 변수에 구분한 구간을 저장한다. 그 후 pthread_create를 통해 thread 2개를 생성하
며 함수는 pthread_ex를 수행하게 하고, point를 arg로 사용한다. 이후 pthread_join 함수를 사
용하여 두 thread가 종료되길 기다리고, 반환되는 값을 status가 가리키도록 한다. Status 1 과
status2를 더한 값을 ret에 저장하고, 사용한 값은 메모리 할당 해제를 진행한다. 그 후 ret를
반환한다.
Main 함수는 다음과 같이 수행된다.
우선 point를 시작과 종료 구간으로 설정한다. 그 후 clock_gettime()을 통해 시작 시간을 측정
한다. Pthread_create를 통해 pthread_ex 함수를 point arg를 사용하여 수행하는 thread를 생성
하고 pthread_join을 통해 이를 기다린다. 이후 종료 시간을 측정하며 thread의 반환값이 저장
된 status와 시간을 출력한다. 마지막으로 status를 메모리 할당 해제 후 프로세스를 종료한다.

다음 파일은 Makefile 파일이다. 이 때 - lrt 옵션을 libraryrt를 사용하여 clock_gettime을 사용하
기 위한 옵션이며, thread target의 경우 - pthread 옵션을 추가해야 컴파일이 가능하다.


다음 결과는 MAX_PROCESSES를 8 로 설정하였을 때의 두 결과를 나타낸다. Fork.c와 thread.c
모두 같은 value값을 나타내며 이는 1 ~ 1 6 까지를 모두 더한 값과 일치하기 때문에 정상적으로
구현됨을 알 수 있다. 두 시간을 비교하였을 때 thread가 fork보다 더 빠르게 수행됨을 확인하
였다.


#### 다음 결과는 MAX_PROCESSES를 64 로 하였을 때의 결과를 나타낸 것이다. 위의 결과에서

fork.c의 값은 64 , thread.c의 값은 8256 으로 차이나 나는 것을 알 수 있다. 이는 exit 시 값의
8 bit만 결과를 받을 수 있기 때문에 2^8의 범위를 초과한 값에서 상위 비트가 누락되어 64 로
계산된 것이다. 실제로 8256 은 16 진수로 2040 으로 타나내며 상위 비트가 누락될 경우 40 - >
64 로 결과가 일치한다. 또한 수행 시간이 더욱 짧아지는 것을 볼 수 있다. 하지만 수행 시간의
경우 process가 적기 때문에 스케줄러에 의해 간간히 fork가 더 빠르게 수행되는 경우가 있다.
기본적으로 fork의 경우 새로운 영역을 생성하는 방면 thread의 경우 기존 heap과 code를 그
대로 사용할 수 있기 때문에 속도의 차이가 난다.

## B. Assignment 3- 2


해당 파일은 해당 과제에서 읽을 파일을 생성하는 filegen.c이다. 해당 프로세스는 우선 temp
디렉토리를 mkdir() 함수를 통해 생성한다. 그 후 chdir() 함수로 현재 working directory를 변경
한다. 그 후 MAX_PROCESSES만큼 반복을 수행한다. sprintf() 함수로 현재 반복자로 이름을 설
정한 후 fopen()을 통해 파일을 생성한다. 그 후 파일에 1 ~ 9 사이의 난수를 작성한다.


해당 파일은 shcedtest.c로 각 스케줄링 별로 성능을 테스트한다.
Struct sched_param param은 priority를 저장하는 변수다. Priority를 각 성능 비교에 맞게 값을
입력한다. 이후 temp directory로 이동한다. Clock_gettime() 함수를 통해 시작 시간을 저장한다.
그 후 for문을 통해 MAX_PROCESSES만큼 프로세스를 생성하며, 해당 생성 프로세서들의
scheduler를 조정한다. 만약 SCHED_OTHER의 경우 nice값을 변경하여 비교를 수행한다. 자식
프로세서들은 sprint()를 통해 설정된 각 인덱스의 파일을 읽는다. 부모 프로세스는 자식 프로세
서의 수만큼 waitpid()를 수행한 후 종료 시간을 측정한다. 그 후 시간을 출력하여 이를 비교한
다.


해당 파일은 Makefile로 filegen.c와 schedtest.c 파일을 컴파일한다. 해당 컴파일에서 Libraryrt
를 사용해야 clock_gettime() 함수를 정상적으로 사용할 수 있기 때문에 - lrt를 추가한다. 프로그
램을 수행할 때의 유의사항은 sudo로 수행해야 priority나 nice 등을 변경하기에 용이하다는 점
이다.


해당 결과는 SCHED_RR(Round Robin)을 사용하며 priority를 각각 min/default/max로 설정하고
실행한 결과다. 다음 결과를 볼 때 min은 1.204초, default는 1. 131 초, max는 1.124초가 나왔다.
Priority가 높아질수록 성능이 개선되는 효과가 나타나는 것을 볼 수 있다. 이는 RR이 Timeslice
에 따라 프로그램을 동작시키기 때문에 파일에 접근하는 동안 다른 프로세스가 Timeslice에 의
해 수행될 수 있기 때문이다. 하지만 위의 성능 개선폭은 점점 작아지는 것을 볼 수 있는데 이
는 priority가 높아질수록 Timeslice의 폭이 작아져 overhead가 커지기 때문이다.

해당 결과는 SCHED_FIFO를 사용하여 priority를 각각 min/default/max로 설정하고 실행한 결과
다. 다음 결과를 볼 때 min은 1. 197 초, default는 1. 157 초, max는 1.158초가 나왔다. FIFO는
preemptive 방식의 스케줄러이며 timeslice를 사용하지 않는다. Timeslice를 사용하지 않기에
priority가 바뀌더라도 실행 시간이 크게 바뀌지 않는 모습을 볼 수 있다.


(^)
해당 결과는 SCHED_OTHER(CFS)를 사용하여 nice 값을 각각 - 20 / 0 / 19 로 설정하고 실행한 결과
다. nice값은 - 20 이 우선 순위가 가장 높으며 +19가 우선순위가 가장 낮다. 또한 0 을 기본으로
한다. 해당 스케줄러는 non-preemptive scheduler이기에 만약 파일 읽기와 같은 접근이 발생할
경우 다른 프로세서가 CPU를 사용할 수 있다. 때문에 각 프로세스가 간단한 파일 읽기만 수행
하는 해당 nice 값의 변화에 따른 큰 성능차이를 보이지 못한다. 하지만 이전 RR이나 FIFO보다
더욱 효율이 높게 프로그램을 수행시킬 수 있다.

## C. Assignment 3- 3


해당 파일은 include/linux/sched.h의 task_struct 구조체의 정의 중 일부를 수정하였다.
Task_struct에서 fork_count 변수를 추가하여 fork 횟수를 확인할 수 있도록 하였다.


해당 파일은 kernel/fork.c 파일에서 _do_fork() 함수의 일부다. 해당 함수는 fork() 실행 시 수행
되는 함수로 추가한 내용으로는 get_current() 매크로를 통해 현재 task_struct를 가져와
fork_count를 증가시킨다. 이후 새롭게 생성된 프로세스에 해당하는 p의 fork_count를 0 으로 초
기화시켜준다.


해당 파일은 입력한 pid에 해당하는 task_struct의 정보를 추적할 수 있는 모듈인
process_tracer.c이다.
해당 모듈 파일에서는 우선 기존 ftrace를 저장할 함수 포인터를 생성한다. 또한 syscall_table을
수정하기 위한 void **를 생성한다. Process_tracer() 함수는 현재 pt_regs의 di로부터 입력한 pid
값을 읽어온다. 그 후 pid_task(find_vpid(), PIDTYPE_PID)함수를 통해 pid에 해당하는 task_struct
를 찾는다. 만약 찾는데 실패한 경우 - 1 을 반환한다. 이후 출력문을 수행한다.
Task_struct의 pid와 이름은 pid, comm에 저장되어 있기에 이를 출력한다. State 변수는 현재 상
태를 나타내며 각 상태는 define되어 있기에 일치하는 경우에 맞게 출력한다. Processor Group
Leader는 group_leader 포인터를 이용해 해당 task_struct에 접근할 수 있다. Context switching
횟수는 nivcsw에 기록되어 있으며 fork 횟수는 이전의 fork_count에 기록되어 있다. Parent
process는 parent 포인터로 접근할 수 있다. Sibling process와 child process를 접근하는 방법은
struct list_head children과 sibling을 이용한다. Sibling은 부모의 children list에 자신을 연결시켜
주는 역할을 수행하며 children은 해당 프로세스의 자식 프로세스들이다. Sibling process를 찾기
위해 list_for_each_entry() 매크로를 사용하여 현재 부모의 자식 프로세스 중 sibling에 해당하는
task를 찾아 현재 task가 아닌 경우에만 출력하도록 한다. Child process의 경우 현 task의
children 중 sibling을 찾는 방법으로 접근한다.

Make_rw와 make_ro는 syscall table을 조작하는 함수로 rw에서는 syscall table을 가져와 읽기
쓰기 권한을 부여한다. Make_ro의 경우 쓰기 권한을 뺏는다. Process_tracer_init()은 Module
initialize 함수다. 해당 함수에서는 기존 syscall table에서 336 번에 해당하는 함수를 임시로 저장
하고 현재 함수로 hijack을 수행한다. Process_tracer_exit()함수에서는 변경했던 함수를 원상복구
한다.


Makefile의 경우 이전과 같이 작성을 수행한다. Obj-m에 모듈 이름을 작성하며 KDIR은
modules 컴파일러의 위치 PWD는 현재 위치를 입력하고 CFLAG로 - DEXPORT_SYMTAB은
global 변수가 없기에 선택사항이다.


다음 프로그램은 임시로 테스트하기 위한 프로그램이다. Fork()의 수가 정상적으로 세지는지 확
인할 수 있다.


다음 결과를 확인하였을 때 위와 같은 출력을 보이고 있다. Pid와 프로그램 명 main이 정상적
으로 출력되며 상태 또한 실행 중으로 정상적으로 보인다. Group leader가 main으로 일치하며
context switch가 사용되지 않을 정도로 짧은 프로그램이기에 0 으로 출력된다. Fork()는 1 번 수
행했기에 1 로 정상 출력되며 parent process는 bash shell에서 수행했기에 정상적으로 출력되었
다. Sibling은 없으며 child 프로세서는 하나 생성되어 있기에 해당 자식이 제대로 출력되었다.


위의 출력은 syscall로 pid에 1 을 넣었을 때의 결과다. 이전과 같이 정상 출력되는 모습이 확인
되며, state가 wait으로 표시되고, context switch, sibling process가 정상적으로 출력되는 것을 검
증할 수 있다. 이를 통해 해당 모듈이 정상 구현되었음을 확인하였다.

## 3. Consideration

```
해당 과제를 통해 thread와 fork의 프로그램 작성 방법을 이해하고 이를 이용한 병렬 연
산을 직접 만들어보는 시간을 보냈다. 특히 fork를 사용할 시 반환되는 값이 8 bit 밖에
```

#### 되지 않기에 프로그램 구상 시 이를 고려해야 한다는 점을 알았다. 그리고

```
get_clocktime() 함수를 사용하여 시간을 clock을 통해 측정하는 방법을 알 수 있었다. 비
록 현재는 chrono를 이용하면 더욱 쉽게 가능하지만 이전에는 어떻게 시간을 측정하는
지 공부할 수 있었다. 2 번 소과제에서는 스케줄러와 priority 그리고 nice값을 바꾸는 함수
를 공부할 수 있었으며 각 값에 따라 어떠한 성능 차이가 있는지 직접 확인할 수 있었다.
마지막 과제에서는 task_struct 구조체에 어떠한 변수가 저장되어 있는지 직접 확인함으
로써 이해할 수 있었던 과제였다.
```
## 4. Reference

#### - 강의자료만을 참고


