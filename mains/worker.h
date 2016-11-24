
/*
 * Abstracts work, i.e. the implementation defines what "work" is.
 *
 * Typically:
 * - flashing an LED
 * - some other transducer, sensing or driving
 *
 * Amount of work is settable.
 * So that work can be used to shed power.
 */


class Worker {
public:
	static void init();
	static void work();

	static void setLeastAmount();
	static void increaseAmount();
	static void decreaseAmount();
};
