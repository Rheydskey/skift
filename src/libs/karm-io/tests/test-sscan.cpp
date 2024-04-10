#include <karm-io/sscan.h>
#include <karm-test/macros.h>

namespace Karm::Io::Tests {

test$(sscanEnded) {
    SScan s{""s};
    expect$(s.ended());

    s = SScan{"a"s};
    expect$(not s.ended());

    return Ok();
}

test$(sscanRem) {
    SScan s{"abc"};
    expect$(s.rem() == 3);

    s = SScan{"abc"};
    s.next();
    expect$(s.rem() == 2);

    s = SScan{"abc"};
    s.next(3);
    expect$(s.rem() == 0);

    return Ok();
}

test$(sscanRemStr) {
    SScan s{"abc"};
    expect$(s.remStr() == "abc");

    s = SScan{"abc"};
    s.next();
    expect$(s.remStr() == "bc");

    s = SScan{"abc"};
    s.next(3);
    expect$(s.remStr() == "");

    return Ok();
}

test$(sscanCurr) {
    SScan s{"abc"};
    expect$(s.curr() == 'a');

    s = SScan{"abc"};
    s.next();
    expect$(s.curr() == 'b');

    s = SScan{"abc"};
    s.next(3);
    expect$(s.curr() == '\0');

    return Ok();
}

test$(sscanPeek) {
    SScan s{"abc"};

    expect$(s.peek() == 'a');
    expect$(s.peek(1) == 'b');
    expect$(s.peek(2) == 'c');
    expect$(s.peek(3) == '\0');
    expect$(s.peek(4) == '\0');

    return Ok();
}

test$(sscanNext) {
    SScan s{"abc"};

    expect$(s.next() == 'a');
    expect$(s.next() == 'b');
    expect$(s.next() == 'c');
    expect$(s.next() == '\0');
    expect$(s.next() == '\0');

    return Ok();
}

test$(sscanSkip) {
    SScan s{"abc"};

    expect$(s.skip('a'));
    expect$(s.rem() == 2);
    expect$(s.skip('b'));
    expect$(s.rem() == 1);
    expect$(s.skip('c'));
    expect$(s.rem() == 0);

    expect$(not s.skip('d'));
    expect$(s.rem() == 0);

    s = SScan{"abc"};
    expect$(s.skip("ab"));
    expect$(s.rem() == 1);
    expect$(s.skip("c"));
    expect$(s.rem() == 0);
    expect$(not s.skip("d"));
    expect$(s.rem() == 0);

    return Ok();
}

test$(sscanEat) {
    SScan s{"abc"};

    expect$(s.eat('a'));
    expect$(s.eat('b'));
    expect$(s.eat('c'));
    expect$(not s.eat('d'));

    s = SScan{"abc"};
    expect$(s.eat("ab"));
    expect$(s.eat("c"));
    expect$(not s.eat("d"));

    s = SScan{"aaaaaa"};
    expect$(s.eat('a'));
    expect$(s.ended());

    return Ok();
}

test$(sscanAhead) {
    SScan s{"abc"};

    expect$(s.ahead('a'));
    expectNot$(s.ahead('b'));
    expect$(s.rem() == 3);

    expect$(s.ahead("ab"));
    expectNot$(s.ahead("bc"));
    expect$(s.rem() == 3);

    return Ok();
}

} // namespace Karm::Io::Tests
