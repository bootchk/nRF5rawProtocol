
/*
 * Abstracts work.
 *
 * Amount of work is settable.
 */


class Worker {
public:
	static void init();
	static void work();

	static void setLeastAmount();
	static void increaseAmount();
	static void decreaseAmount();
};
