#include <iostream>
#include <vector>
#include <string>
#include <map>

// 1. PotionRecipe: 물약 데이터 모델
class PotionRecipe {
public:
    PotionRecipe(const std::string& name, const std::vector<std::string>& ingredients)
        : name_(name), ingredients_(ingredients) {
    }

    const std::string& GetName() const { return name_; }
    const std::vector<std::string>& GetIngredients() const { return ingredients_; }

private:
    std::string name_;
    std::vector<std::string> ingredients_;
};

// 2. RecipeManager: 레시피 저장 및 검색 담당
class RecipeManager {
public:
    PotionRecipe* AddRecipe(const std::string& name, const std::vector<std::string>& ingredients) {
        // 중복 체크 
        if (FindRecipeByName(name) != nullptr) return nullptr;

        recipes_.push_back(PotionRecipe(name, ingredients));
        return &recipes_.back();
    }

    PotionRecipe* FindRecipeByName(const std::string& name) {
        for (auto& r : recipes_) {
            if (r.GetName() == name) return &r;
        }
        return nullptr;
    }

    std::vector<PotionRecipe> FindRecipesByIngredient(const std::string& ingredient) const {
        std::vector<PotionRecipe> result;
        for (const auto& r : recipes_) {
            for (const auto& ing : r.GetIngredients()) {
                if (ing == ingredient) {
                    result.push_back(r);
                    break;
                }
            }
        }
        return result;
    }

    const std::vector<PotionRecipe>& GetAllRecipes() const { return recipes_; }

private:
    std::vector<PotionRecipe> recipes_;
};

// 3. StockManager: 재고 및 수량 제한
class StockManager {
public:
    static constexpr int MAX_STOCK = 3;

    // [도전 1] 레시피 추가 시 자동 재고 3개 설정
    void InitializeStock(const std::string& potionName) {
        potionStock_[potionName] = MAX_STOCK;
    }

    // [도전 2] 재고가 1개 이상일 때만 지급 (1 감소)
    bool DispensePotion(const std::string& potionName) {
        if (potionStock_.count(potionName) > 0 && potionStock_[potionName] > 0) {
            potionStock_[potionName]--;
            return true;
        }
        return false;
    }

    // [도전 3, 4] 공병 반환 (MAX_STOCK 3개 초과 불가)
    void ReturnPotion(const std::string& potionName) {
        if (potionStock_.count(potionName) > 0) {
            if (potionStock_[potionName] < MAX_STOCK) {
                potionStock_[potionName]++;
                std::cout << ">> [" << potionName << "] 반환 성공. 현재: " << potionStock_[potionName] << std::endl;
            }
            else {
                std::cout << ">> [경고] " << potionName << " 창고가 이미 가득 찼습니다! (MAX 3)" << std::endl;
            }
        }
    }

    int GetStock(const std::string& potionName) const {
        auto it = potionStock_.find(potionName);
        return (it != potionStock_.end()) ? it->second : 0;
    }

private:
    std::map<std::string, int> potionStock_;
};

// 4. AlchemyWorkshop: 
class AlchemyWorkshop {
public:
    void AddRecipe(const std::string& potionName, const std::vector<std::string>& ingredients) {
        if (recipeManager_.AddRecipe(potionName, ingredients)) {
            stockManager_.InitializeStock(potionName);
            std::cout << ">> '" << potionName << "' 레시피 추가 및 초기 재고 3개 설정 완료.\n";
        }
    }

    void DisplayAllRecipes() const {
        const auto& recipes = recipeManager_.GetAllRecipes();
        if (recipes.empty()) return;

        for (const auto& r : recipes) {
            std::cout << "- " << r.GetName() << " [재고: " << stockManager_.GetStock(r.GetName()) << "개] | 재료: ";
            for (const auto& ing : r.GetIngredients()) std::cout << ing << " ";
            std::cout << std::endl;
        }
    }

    int GetStockByName(const std::string& potionName) const {
        return stockManager_.GetStock(potionName);
    }

    bool DispensePotionByName(const std::string& potionName) {
        return stockManager_.DispensePotion(potionName);
    }

    // [도전 2] 재료명으로 창고를 검색하여 재고가 있는 물약들을 지급
    std::vector<std::string> DispensePotionsByIngredient(const std::string& ingredient) {
        std::vector<std::string> dispensedList;
        std::vector<PotionRecipe> matches = recipeManager_.FindRecipesByIngredient(ingredient);

        for (const auto& r : matches) {
            if (stockManager_.DispensePotion(r.GetName())) {
                dispensedList.push_back(r.GetName());
            }
        }
        return dispensedList;
    }

    void ReturnPotionByName(const std::string& potionName) {
        stockManager_.ReturnPotion(potionName);
    }

private:
    RecipeManager recipeManager_;
    StockManager stockManager_;
};

int main() {
    AlchemyWorkshop workshop;

    workshop.AddRecipe("Healing Potion", { "Herb", "Water" });
    workshop.AddRecipe("Mana Potion", { "Magic Water", "Crystal" });
    workshop.AddRecipe("Stamina Potion", { "Herb", "Berry" });
    workshop.AddRecipe("Fire Resistance Potion", { "Fire Flower", "Ash" });

    std::cout << "\n=== 초기 상태 (자동 재고 3개 확인) ===\n";
    workshop.DisplayAllRecipes();

    std::cout << "\n=== 이름으로 지급 테스트 ===\n";
    for (int i = 1; i <= 4; ++i) {
        std::cout << i << "회 지급 시도: " << (workshop.DispensePotionByName("Healing Potion") ? "성공" : "실패") << "\n";
    }

    std::cout << "\n=== 재료로 지급 테스트 (Herb) ===\n";
    std::vector<std::string> dispensed = workshop.DispensePotionsByIngredient("Herb");
    std::cout << "지급된 물약 수: " << dispensed.size() << "\n";
    for (const std::string& name : dispensed) std::cout << "- " << name << "\n";

    std::cout << "\n=== 공병 반환 테스트 (MAX 3 제한 확인) ===\n";
    workshop.ReturnPotionByName("Healing Potion"); // 0 -> 1
    workshop.ReturnPotionByName("Healing Potion"); // 1 -> 2
    workshop.ReturnPotionByName("Healing Potion"); // 2 -> 3
    workshop.ReturnPotionByName("Healing Potion"); // 3 -> 실패(MAX 3)

    std::cout << "\n=== 최종 상태 ===\n";
    workshop.DisplayAllRecipes();

    return 0;
}