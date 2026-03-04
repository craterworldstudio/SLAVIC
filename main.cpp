int main() {
    Mind mind;
    mind.load_state();

    while (mind.is_running()) {
        mind.tick();
    }

    mind.save_state();
}