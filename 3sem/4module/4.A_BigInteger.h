/*!
 * В этой задаче разрешается подключать
 * <iostream>, <vector> и <string> и только их.
 *
 * Напишите класс BigInteger для работы с длинными целыми числами.
 * Должны поддерживаться операции:
 *
 *   сложение, вычитание, умножение, деление, остаток по модулю,работающие
 *   так же, как и для int; составное присваивание с этими операциями;
 *   унарный минус, префиксный и постфиксный инкремент и декремент,
 *   операторы сравнения == != < > <= >=
 *   вывод в поток и ввод из потока (возможен ввод из пустого потока);
 *   метод toString(), возвращающий строковое представление числа;
 *   конструирование из int (в том числе неявное преобразование, когда это надо);
 *   неявное преобразование в bool, когда это надо (должно работать в условных выражениях).
 *
 * Асимптотические ограничения на время работы операторов в контесте
 * не проверяются, но реализация должна работать за:
 *
 *   Сложение, вычитание, унарный минус, операторы сравнения – O(n),
 *   Умножение - o(n1.6),
 *   Деление, остаток по модулю - O(n^2)
 *
 * где n – количество разрядов большего числа (по модулю).
 *
 * В вашем файле должна отсутствовать функция main(),
 * а сам файл должен называться BigInteger.h.
 * В качестве компилятора необходимо указывать Make.
 *
 * Ваш код будет вставлен посредством команды #include<BigInteger.h>
 * в программу, содержащую тесты; вследствие этого, код необходимо
 * отправлять в файле со строго соответствующим именем!
 */

#include <iostream>
#include <vector>
#include <string>


constexpr size_t kMulConstant = 8;


class BigInteger final {
 public:
  template<class CharT, class Traits>
  friend std::basic_istream<CharT, Traits>&
  operator>>(std::basic_istream<CharT, Traits>& stream, BigInteger& bigint);

  BigInteger();
  BigInteger(const BigInteger&) = default;
  BigInteger(BigInteger&&) = default;
  BigInteger(int number);
  ~BigInteger() = default;
  BigInteger& operator= (const BigInteger&) = default;
  BigInteger& operator= (BigInteger&& other) = default;
  BigInteger& operator+=(const BigInteger& other);
  BigInteger& operator-=(const BigInteger& other);
  BigInteger& operator*=(const BigInteger& other);
  BigInteger& operator/=(const BigInteger& other);
  BigInteger& operator%=(const BigInteger& other);
  const BigInteger operator-() const;
  BigInteger& operator++();
  BigInteger& operator--();
  const BigInteger operator++(int);
  const BigInteger operator--(int);
  bool operator==(const BigInteger& other) const noexcept;
  bool operator!=(const BigInteger& other) const noexcept;
  bool operator< (const BigInteger& other) const noexcept;
  bool operator> (const BigInteger& other) const noexcept;
  bool operator<=(const BigInteger& other) const noexcept;
  bool operator>=(const BigInteger& other) const noexcept;
  std::string toString() const;
  explicit operator int() const;
  explicit operator bool() const;
private:
  bool sign_ = false;
  size_t ExpandedToPowerOfTwoSize() const;
  void Shrink();
  void Shift(int shift);
  void MulByDigit(int digit);
  void UnsignedAdd(const BigInteger& other);
  void UnsignedSubtract(const BigInteger& other);
  void SquareMul(const BigInteger& other);
  static BigInteger RecursiveKaratsuba(BigInteger lhvalue,
                                       BigInteger rhvalue);
  static int SignedCompare(const BigInteger& lhvalue,
                           const BigInteger& rhvalue);
  static int UnsignedCompare(const BigInteger& lhvalue,
                             const BigInteger& rhvalue);
  constexpr size_t kBase = 10;
  std::vector<uint8_t> number_;
};

BigInteger operator+(BigInteger lhvalue, const BigInteger& rhvalue);
BigInteger operator-(BigInteger lhvalue, const BigInteger& rhvalue);
BigInteger operator*(BigInteger lhvalue, const BigInteger& rhvalue);
BigInteger operator/(BigInteger lhvalue, const BigInteger& rhvalue);
BigInteger operator%(BigInteger lhvalue, const BigInteger& rhvalue);

template<class CharT, class Traits>
std::basic_istream<CharT, Traits>&
operator>>(std::basic_istream<CharT, Traits>&, BigInteger& bigint);

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& stream, const BigInteger& bigint);

///////////////////////////////////////////////////////////////////////////////

BigInteger::BigInteger() : number_(1, 0) {}

BigInteger::BigInteger(int number) {
  if (number == 0) {
    number_.push_back(0);
    return;
  }

  if (number < 0) {
    sign_ = true;
    number *= -1;
  }

  while (number != 0) {
    number_.push_back(number % kBase);
    number /= kBase;
  }
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
  if (this->sign_ != other.sign_) {
    if (UnsignedCompare(*this, other) >= 0) {
      UnsignedSubtract(other);
    } else {
      BigInteger other_copy = other;
      other_copy.UnsignedSubtract(*this);
      *this = std::move(other_copy);
    }
    if (this->number_.size() == 1 && this->number_.at(0) == 0) {
        this->sign_ = false;
    }
  } else {
    UnsignedAdd(other);
  }

  return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& other) {
  if (this->sign_ != other.sign_) {
    UnsignedAdd(other);
  } else {
    if (UnsignedCompare(*this, other) >= 0) {
      UnsignedSubtract(other);
    } else {
      BigInteger other_copy = other;
      other_copy.UnsignedSubtract(*this);
      other_copy.sign_ = !other.sign_;
      *this = std::move(other_copy);
    }
    if (this->number_.size() == 1 && this->number_.at(0) == 0) {
        this->sign_ = false;
    }
  }

  return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
  size_t new_size =
    this->ExpandedToPowerOfTwoSize() > other.ExpandedToPowerOfTwoSize() ?
    this->ExpandedToPowerOfTwoSize() : other.ExpandedToPowerOfTwoSize();

  BigInteger other_copy = other;

  this->number_.resize(new_size);
  other_copy.number_.resize(new_size);

  bool new_sign = this->sign_ != other.sign_;

  this->sign_ = false;
  other_copy.sign_ = false;

  *this = RecursiveKaratsuba(std::move(*this), std::move(other_copy));
  this->sign_ = new_sign;
  this->Shrink();
  this->number_.shrink_to_fit();

  if (this->number_ == std::vector<uint8_t>({0})) {
    this->sign_ = false;
  }

  return *this;
}


/*
 * цикл (1) выполнится не более n = max(len(left), len(right)) раз
 * цикл (2) выполнится не более kBase = const раз,
 * внутри операции выполняются за O(n)
 * операции (3) занимают O(n) времени
 * Итоговое время работы n * (const * O(n) + O(n)) = O(n^2)
 */
BigInteger& BigInteger::operator/=(const BigInteger& other) {
  if (other == 0)
    throw std::logic_error("division by zero");

  BigInteger this_copy = *this;
  BigInteger other_copy = other;
  other_copy.Shift(this_copy.number_.size() - other_copy.number_.size());
  BigInteger current_shifted_one = BigInteger(1);

  if (this->number_.size() < other.number_.size()) {
    *this = 0;
    return *this;
  }

  current_shifted_one.Shift(this->number_.size() - other.number_.size());
  BigInteger answer = 0;
  /* (1) */
  for (int64_t i = this_copy.number_.size() - other.number_.size();
       i >= 0 ;
       --i) {
    /* (2) */
    while (UnsignedCompare(this_copy, other_copy) >= 0) {
      this_copy.UnsignedSubtract(other_copy);
      answer.UnsignedAdd(current_shifted_one);
    }
    /* (3) */
    current_shifted_one.Shift(-1);
    other_copy.Shift(-1);
  }

  answer.sign_ = answer == 0 ? false : this->sign_ != other.sign_;
  *this = std::move(answer);
  return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
  bool res_sign = this->sign_;

  BigInteger this_copy = *this;
  this_copy.sign_ = false;
  BigInteger other_copy = other;
  other_copy.sign_ = false;
  this_copy /= other_copy;
  this_copy *= other_copy;

  this->UnsignedSubtract(this_copy);
  this->sign_ = this->number_ == std::vector<uint8_t>{0} ? false : res_sign;
  return *this;
}

const BigInteger BigInteger::operator-() const {
  BigInteger this_copy = *this;
  if (this_copy != 0) this_copy.sign_ = !this_copy.sign_;
  return this_copy;
}

BigInteger& BigInteger::operator++() {
  *this += 1;
  return *this;
}

BigInteger& BigInteger::operator--() {
  *this -= 1;
  return *this;
}

const BigInteger BigInteger::operator++(int) {
  BigInteger this_copy = *this;
  *this += 1;
  return this_copy;
}

const BigInteger BigInteger::operator--(int) {
  BigInteger this_copy = *this;
  *this -= 1;
  return this_copy;
}

bool BigInteger::operator==(const BigInteger& other) const noexcept {
  return SignedCompare(*this, other) == 0;
}

bool BigInteger::operator!=(const BigInteger& other) const noexcept {
  return SignedCompare(*this, other) != 0;
}

bool BigInteger::operator<(const BigInteger& other) const noexcept {
  return SignedCompare(*this, other) < 0;
}

bool BigInteger::operator>(const BigInteger& other) const noexcept {
  return SignedCompare(*this, other) > 0;
}

bool BigInteger::operator<=(const BigInteger& other) const noexcept {
  return SignedCompare(*this, other) <= 0;
}

bool BigInteger::operator>=(const BigInteger& other) const noexcept {
  return SignedCompare(*this, other) >= 0;
}

std::string BigInteger::toString() const {
  std::string string_representation;
  if (sign_)
    string_representation.push_back('-');

  for (size_t i = 0; i < number_.size(); ++i) {
    string_representation.push_back(number_[number_.size() - i - 1] + '0');
  }

  return string_representation;
}

BigInteger::operator int() const {
  int value = 0;

  int multiplier = 1;
  for (auto digit: number_) {
    value += multiplier * digit;
    multiplier *= kBase;
  }

  if (sign_)
    value *= -1;

  return value;
}

BigInteger::operator bool() const {
  return !(number_.size() == 1 && number_[0] == 0);
}

size_t BigInteger::ExpandedToPowerOfTwoSize() const {
  auto most_significant_bit = [](size_t number) -> size_t {
    for (size_t i = 63; i > 0; --i) {
      if (number >> i) return i;
    }

    return 0;
  };

  auto expanded_size =
    [&most_significant_bit](const BigInteger& bigint) -> size_t {
    if ((1ull << most_significant_bit(bigint.number_.size())) ==
         bigint.number_.size())
      return bigint.number_.size();
    else
      return 1ull << (most_significant_bit(bigint.number_.size()) + 1);
  };

  return expanded_size(*this);
}

void BigInteger::Shrink() {
  size_t zero_pos = number_.size();
  for (int64_t i = number_.size() - 1; i >= 0; --i) {
    if (number_[i] == 0) {
      zero_pos = i;
    } else {
      break;
    }
  }

  number_.resize(zero_pos ? zero_pos : 1);
}

void BigInteger::Shift(int shift) {
  if (shift < 0) {
    shift *= -1;
    for (auto from_it = number_.begin() + shift,
              to_it   = number_.begin();
         from_it != number_.end();
         ++from_it,
         ++to_it) {
      *to_it = *from_it;
    }
    number_.resize(number_.size() - shift);
  } else {
    number_.resize(number_.size() + shift, 0);
    for (auto from_it = number_.rbegin() + shift,
              to_it   = number_.rbegin();
         from_it != number_.rend();
         ++from_it,
         ++to_it) {
      *to_it = *from_it;
    }
    for (size_t i = 0; static_cast<int>(i) < shift; ++i) {
      number_[i] = 0;
    }
  }
}

void BigInteger::MulByDigit(int digit) {
  int carry = 0;
  for (size_t i = 0; i < number_.size(); ++i) {
    int64_t multiplication = number_[i] * digit;
    multiplication += carry;
    number_[i] = multiplication % kBase;
    carry = multiplication / kBase;
  }

  if (carry != 0)
    number_.push_back(carry);
}

void BigInteger::UnsignedAdd(const BigInteger& other) {
  uint64_t carry = 0;
  size_t other_size = other.number_.size();
  for (size_t i = 0; i < other_size; ++i) {
    if (i == this->number_.size() - 1)
      this->number_.push_back(0);

    uint64_t sum = this->number_[i] + other.number_[i] + carry;
    this->number_[i] = sum % kBase;
    carry = sum / kBase;
  }

  for (size_t i = other_size; carry != 0; ++i) {
    if (this->number_.size() == i)
      this->number_.push_back(0);

    uint64_t sum = this->number_[i] + carry;
    this->number_[i] = sum % kBase;
    carry = sum / kBase;
  }

  this->Shrink();
}

/*!
 * @brief *this >= other guaranteed
 */
void BigInteger::UnsignedSubtract(const BigInteger& other) {
  for (size_t i = 0; i < other.number_.size(); ++i) {
    uint64_t minuend = this->number_[i];
    if (this->number_[i] < other.number_[i]) {
      minuend += kBase;

      size_t j = i + 1;
      while (this->number_[j] == 0) ++j;
      for (size_t k = i + 1; k < j; ++k)
        this->number_[k] = kBase - 1;
      --this->number_[j];
    }

    this->number_[i] = minuend - other.number_[i];
  }

  this->Shrink();
}

void BigInteger::SquareMul(const BigInteger& other) {
  BigInteger result = 0;
  BigInteger other_copy = other;
  for (size_t i = 0; i < this->number_.size(); ++i) {
    other_copy = other;
    other_copy.MulByDigit(this->number_[i]);
    other_copy.Shift(i);
    other_copy.Shrink();
    result += other_copy;
  }

  *this = std::move(result);
}

/*!
 * @brief lhvalue.size() == rhvalue.size() == 2^k for some k
 */
BigInteger BigInteger::RecursiveKaratsuba(BigInteger lhvalue,
                                          BigInteger rhvalue) {
  size_t number_size = lhvalue.number_.size();

  if (lhvalue.number_.size() <= kMulConstant) {
    lhvalue.Shrink();
    rhvalue.Shrink();
    lhvalue.SquareMul(rhvalue);
    return std::move(lhvalue);
  }

  BigInteger lhvalue_most_significant,
             lhvalue_least_singificant,
             rhvalue_most_significant,
             rhvalue_least_significant;
  lhvalue_most_significant.number_ =
    std::vector<uint8_t>(
      lhvalue.number_.begin() + number_size / 2,
      lhvalue.number_.end());
  lhvalue_most_significant.Shrink();

  lhvalue_least_singificant.number_ =
    std::vector<uint8_t>(
      lhvalue.number_.begin(),
      lhvalue.number_.begin() + number_size / 2);
  lhvalue_least_singificant.Shrink();

  rhvalue_most_significant.number_ =
    std::vector<uint8_t>(
      rhvalue.number_.begin() + number_size / 2,
      rhvalue.number_.end());
  rhvalue_most_significant.Shrink();

  rhvalue_least_significant.number_ =
    std::vector<uint8_t>(
      rhvalue.number_.begin(),
      rhvalue.number_.begin() + number_size / 2);
  rhvalue_least_significant.Shrink();

  BigInteger lm_mul_rm(lhvalue_most_significant);
  lm_mul_rm *= rhvalue_most_significant;

  BigInteger ll_mul_rl(lhvalue_least_singificant);
  ll_mul_rl *= rhvalue_least_significant;

  BigInteger lm_plus_ll_mul_rm_plus_rl(
    lhvalue_most_significant + lhvalue_least_singificant);
  lm_plus_ll_mul_rm_plus_rl *= (
    rhvalue_most_significant + rhvalue_least_significant);

  BigInteger lm_mul_rl_plus_ll_mul_rm = std::move(lm_plus_ll_mul_rm_plus_rl);
  lm_mul_rl_plus_ll_mul_rm -= lm_mul_rm;
  lm_mul_rl_plus_ll_mul_rm -= ll_mul_rl;

  lm_mul_rm.Shift(number_size);
  lm_mul_rl_plus_ll_mul_rm.Shift(number_size / 2);
  ll_mul_rl += lm_mul_rm;
  ll_mul_rl += lm_mul_rm;
  return ll_mul_rl;
}

/*!
 *
 * @return returns 0                if lhvalue == rhvalue
 *                 positive integer if lhvalue > rhvalue
 *                 negative integer if lhvalue < rhvalue
 */
int BigInteger::SignedCompare(const BigInteger& lhvalue,
                              const BigInteger& rhvalue) {
  if (lhvalue.sign_ != rhvalue.sign_)
    return !lhvalue.sign_ ? 1 : -1;

  if (!lhvalue.sign_)
    return UnsignedCompare(lhvalue, rhvalue);

  return -UnsignedCompare(lhvalue, rhvalue);
}

/*!
 * @return returns 0                if lhvalue == rhvalue
 *                 positive integer if lhvalue > rhvalue
 *                 negative integer if lhvalue < rhvalue
 */
int BigInteger::UnsignedCompare(const BigInteger& lhvalue,
                                const BigInteger& rhvalue) {
  if (lhvalue.number_.size() != rhvalue.number_.size())
    return lhvalue.number_.size() > rhvalue.number_.size() ? 1 : -1;

  for (auto left_it  = lhvalue.number_.crbegin(),
            right_it = rhvalue.number_.crbegin();
            left_it  != lhvalue.number_.crend();
            ++left_it,
            ++right_it) {
    if (*left_it != *right_it)
      return *left_it > *right_it ? 1 : -1;
  }

  return 0;
}

BigInteger operator+(BigInteger lhvalue, const BigInteger& rhvalue) {
  lhvalue += rhvalue;
  return lhvalue;
}

BigInteger operator-(BigInteger lhvalue, const BigInteger& rhvalue) {
  lhvalue -= rhvalue;
  return lhvalue;
}

BigInteger operator*(BigInteger lhvalue, const BigInteger& rhvalue) {
  lhvalue *= rhvalue;
  return lhvalue;
}

BigInteger operator/(BigInteger lhvalue, const BigInteger& rhvalue) {
  lhvalue /= rhvalue;
  return lhvalue;
}

BigInteger operator%(BigInteger lhvalue, const BigInteger& rhvalue) {
  lhvalue %= rhvalue;
  return lhvalue;
}

template<class CharT, class Traits>
std::basic_istream<CharT, Traits>&
operator>>(std::basic_istream<CharT, Traits>& stream, BigInteger& bigint) {

  std::string number_representation;
  stream >> number_representation;

  if (number_representation.empty()) {
    bigint = 0;
    return stream;
  }

  bigint = 0;
  size_t first_digit = number_representation[0] == '-' || number_representation[0] == '+'

      ? 1 : 0;

  size_t multiplier = 0;
  for (int64_t i = number_representation.size() - 1;
       i >= static_cast<int64_t>(first_digit);
       --i) {

    BigInteger x = number_representation[i] - '0';
    x.Shift(multiplier);
    bigint += x;
    multiplier += 1;
  }

  if (first_digit == 1 && number_representation[0] == '-')
    bigint.sign_ = true;

  if (bigint.number_.size() == 1 && bigint.number_[0] == 0)
    bigint.sign_ = false;

  return stream;
}

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& stream,
           const BigInteger& bigint) {
  return stream << bigint.toString();
}
