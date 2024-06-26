#include <stdio.h>		// print()
#include <stdlib.h>		// malloc()
#include <string.h>		// memcpy(), strcpy(), strcmp()
#include <unistd.h>		// sleep()
#include <sys/shm.h>	// shmget(), shmat(), shmdt(), shmctl()

#define true 1
#define false 0

#define KEY_NUM 3306
#define MEM_SIZE 1024
#define MSG_SIZE 10

int main(void) {
	int shm_id;
	void *shm_addr;
	char *msg;

	shm_id = shmget((key_t) KEY_NUM, MEM_SIZE, IPC_CREAT | 0666);
	if (shm_id == -1) {
		printf("%s: failed to get shared memory\n", __func__);
		return 0;
	}

	shm_addr = shmat(shm_id, (void *) 0, 0);
	if (shm_addr == (void *) -1) {
		printf("%s: failed to attach shared memory to process\n", __func__);
		return 0;
	}

	printf("%s: shared memory address = %p\n", __func__, shm_addr);

	msg = malloc(MSG_SIZE);
	if (!msg) {
		printf("%s: failed to malloc msg\n", __func__);
		return 0;
	}

	memset(msg, 0, MSG_SIZE);
	strcpy(msg, "close");
	memcpy((void *) shm_addr, msg, MSG_SIZE);
	printf("%s: send request to the other process\n", __func__);

	while (true) {
		memcpy(msg, (void *) shm_addr, MSG_SIZE);
		if (!strcmp(msg, "done")) {
			printf("%s: the other process got message!\n", __func__);
			break;
		}

		sleep(1);
		printf("%s: the other process didn't get message yet\n", __func__);
	}
	
	shmdt(shm_addr);
	if (shmctl(shm_id, IPC_RMID, 0) == -1) {
		printf("%s: failed to remove shared memory\n", __func__);
		return 0;
	}

	return 0;
}
