#include <sup/dto/anyvalue.h>
#include <sup/dto/any_functor.h>

class RPCSniffServer : public sup::dto::AnyFunctor
{
public:
  RPCSniffServer(const sup::dto::AnyValue& reply);
  ~RPCSniffServer() override;

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& input) override;

private:
  sup::dto::AnyValue m_reply;
};
