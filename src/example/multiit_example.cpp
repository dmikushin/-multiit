#include <multiit/multiit.h>

int main(int argc, char* argv[])
{
	multiit::runtime::MultiIterator mi({ 2, 3, 4 });
	// OR: multiit::compiletime::MultiIterator<2, 3, 4> mi;

	int niters = 0;
	const int size = mi.getSize();
	while (1)
	{
		niters++;
		if (!mi.next()) break;

		// TODO Use the current combination of choices in a target app.
		const auto& current = mi.getCurrent();
		for (int i = 0; i < size; i++)
			printf("%d ", current[i]);
		printf("\n");
	}

	printf("%d iterations visited\n", niters);

	return 0;
}

